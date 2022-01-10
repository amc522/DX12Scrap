#include "d3d12/D3D12UploadBufferPool.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
void UploadBufferPool::init()
{
    pushBackBuffer();
}

void UploadBufferPool::update()
{
    for(UploadBuffer& buffer : mUploadBuffers)
    {
        buffer.byteOffset = 0;
    }
}

UploadBufferMap UploadBufferPool::map(size_t bytes)
{
    for(size_t i = 0; i < mUploadBuffers.size(); ++i)
    {
        UploadBuffer& uploadBuffer = mUploadBuffers[i];

        const size_t remainingByteSize = uploadBuffer.byteSize - uploadBuffer.byteOffset;
        if(remainingByteSize >= bytes)
        {
            UploadBufferMap bufferMap = mapUnchecked(i, bytes);

            if(bufferMap.writeBuffer.empty()) { continue; }
        }
    }

    pushBackBuffer(std::max(bytes, sMinBufferByteSize));

    return mapUnchecked(mUploadBuffers.size() - 1, bytes);
}

UploadBufferCopyData UploadBufferPool::unmap(const UploadBufferMap& bufferMap)
{
    UploadBuffer& uploadBuffer = mUploadBuffers[bufferMap.bufferIndex];

    UploadBufferCopyData copyData;
    copyData.buffer = uploadBuffer.buffer.Get();
    copyData.byteSize = bufferMap.writeBuffer.size();
    copyData.byteOffset = bufferMap.byteOffset;

    D3D12_RANGE range;
    range.Begin = bufferMap.byteOffset;
    range.End = bufferMap.writeBuffer.size_bytes();

    uploadBuffer.buffer->Unmap(0, &range);

    return copyData;
}

UploadBufferMap UploadBufferPool::mapUnchecked(size_t index, size_t byteSize)
{
    UploadBuffer& uploadBuffer = mUploadBuffers[index];

    D3D12_RANGE range;
    range.Begin = uploadBuffer.byteOffset;
    range.End = uploadBuffer.byteOffset + byteSize;

    void* data;
    HRESULT hr = uploadBuffer.buffer->Map(0, &range, &data);

    if(FAILED(hr)) { return {}; }

    UploadBufferMap bufferMap;
    bufferMap.bufferIndex = index;
    bufferMap.writeBuffer = std::span<std::byte>(reinterpret_cast<std::byte*>(data), byteSize);
    bufferMap.byteOffset = uploadBuffer.byteOffset;

    uploadBuffer.byteOffset += byteSize;

    return bufferMap;
}

void UploadBufferPool::pushBackBuffer(size_t byteSize)
{
    ID3D12Device* device = d3d12::DeviceContext::instance().getDevice();

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Alignment = 0;
    bufferDesc.Width = byteSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.SampleDesc.Quality = 0;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_RESOURCE_ALLOCATION_INFO allocInfo = device->GetResourceAllocationInfo(0, 1, &bufferDesc);
    bufferDesc.Alignment = allocInfo.Alignment;
    bufferDesc.Width = allocInfo.SizeInBytes;

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    uploadHeapProps.CreationNodeMask = 0;
    uploadHeapProps.VisibleNodeMask = 0;

    ComPtr<ID3D12Resource> uploadResource;
    HRESULT hr =
        device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadResource));
    if(FAILED(hr))
    {
        spdlog::critical("Failed to create upload buffer for UploadBufferPool.");
        return;
    }

    mUploadBuffers.emplace_back(std::move(uploadResource), bufferDesc.Width);
}
} // namespace scrap::d3d12