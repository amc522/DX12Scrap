#include "d3d12/D3D12Buffer.h"

#include "Utility.h"
#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <gpufmt/dxgi.h>
#include <gpufmt/traits.h>
#include <spdlog/spdlog.h>

#include <ostream>
using namespace Microsoft::WRL;

namespace scrap::d3d12
{
Buffer::~Buffer()
{
    DeviceContext::instance().queueResourceForDestruction(std::move(mResource), std::move(mDescriptorHeapReservation), mLastUsedFrameCode);
}

std::optional<Buffer::Error> Buffer::init(const SimpleParams& params, nonstd::span<std::byte> buffer)
{
    return initInternal(params, buffer);
}

std::optional<Buffer::Error> Buffer::init(const FormattedParams& params, nonstd::span<std::byte> buffer)
{
    return initInternal(params, buffer);
}

std::optional<Buffer::Error> Buffer::init(const StructuredParams& params, nonstd::span<std::byte> buffer)
{
    return initInternal(params, buffer);
}

D3D12_CPU_DESCRIPTOR_HANDLE Buffer::getSrvCpu() const
{
    return mDescriptorHeapReservation.getCpuHandle(mSrvIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE Buffer::getSrvGpu() const
{
    return mDescriptorHeapReservation.getGpuHandle(mSrvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE Buffer::getUavCpu() const
{
    return mDescriptorHeapReservation.getCpuHandle(mUavIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE Buffer::getUavGpu() const
{
    return mDescriptorHeapReservation.getGpuHandle(mUavIndex);
}

D3D12_INDEX_BUFFER_VIEW Buffer::getIndexView() const
{
    return getIndexView(mParams.dxgiFormat);
}

D3D12_INDEX_BUFFER_VIEW Buffer::getIndexView(DXGI_FORMAT format) const
{
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = mResource->GetGPUVirtualAddress();
    ibv.Format = format;
    ibv.SizeInBytes = mParams.elementByteSize;
    return ibv;
}

bool Buffer::isReady() const
{
    return mResource != nullptr && mUploadFrameCode <= DeviceContext::instance().getCopyContext().getLastCompletedFrameCode();
}

void Buffer::markAsUsed()
{
    mLastUsedFrameCode = DeviceContext::instance().getCurrentFrameCode();
}

std::optional<Buffer::Error> Buffer::initInternal(Params params, nonstd::span<std::byte> buffer)
{
    DeviceContext& deviceContext = DeviceContext::instance();

    if(params.type == Type::Formatted)
    {
        const auto formatTranslation = gpufmt::dxgi::translateFormat(params.format);
        if(!formatTranslation || !formatTranslation.exact) { return Error::InvalidFormat; }
        params.dxgiFormat = formatTranslation.exact.value();

        const auto& formatInfo = gpufmt::formatInfo(params.format);
        params.byteSize = formatInfo.blockByteSize * params.numElements;
        params.elementByteSize = formatInfo.blockByteSize;
    }
    else if(params.type == Type::Structured)
    {
        params.dxgiFormat = DXGI_FORMAT_UNKNOWN;
        params.byteSize = params.numElements * params.elementByteSize;
    }
    else
    {
        params.dxgiFormat = DXGI_FORMAT_UNKNOWN;
        params.elementByteSize = 1;
    }

    D3D12_RESOURCE_ALLOCATION_INFO allocInfo;

    //========================
    // Create buffer resource
    //========================
    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Alignment = 0;
    bufferDesc.Width = params.byteSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.SampleDesc.Quality = 0;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if((params.accessFlags & ResourceAccessFlags::GpuRead) != ResourceAccessFlags::GpuRead)
    {
        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    }
    if((params.accessFlags & ResourceAccessFlags::GpuWrite) == ResourceAccessFlags::GpuWrite)
    {
        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    // Here's and article describing in detail what this memory alignment stuff is all about:
    // https://asawicki.info/news_1726_secrets_of_direct3d_12_resource_alignment

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-getresourceallocationinfo
    allocInfo = deviceContext.getDevice()->GetResourceAllocationInfo(0, 1, &bufferDesc);
    bufferDesc.Alignment = allocInfo.Alignment;

    // The destination heap of the buffer. This is the heap where the buffer will be for its lifetime on the
    // gpu.
    D3D12_HEAP_PROPERTIES defaultHeapProps = {};
    defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    defaultHeapProps.CreationNodeMask = 0;
    defaultHeapProps.VisibleNodeMask = 0;

    // Maybe use D3D12_RESOURCE_STATE_COMMON when not copy the texture???
    D3D12_RESOURCE_STATES initialResourceState =
        (!buffer.empty()) ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommittedresource
    HRESULT hr = deviceContext.getDevice()->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE,
                                                                    &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST,
                                                                    nullptr, IID_PPV_ARGS(&mResource));

    if(FAILED(hr)) { return Error::FailedToCreateGpuResource; }

    std::wstring wideName;
    int wideStrSize = MultiByteToWideChar(CP_UTF8, 0u, params.name.data(), (int)params.name.size(), nullptr, 0);
    wideName.reserve(wideStrSize + sizeof(" (Upload)"));
    wideName.resize(wideStrSize);
    MultiByteToWideChar(CP_UTF8, 0u, params.name.data(), (int)params.name.size(), wideName.data(),
                        (int)wideName.size());
    wideName.data()[wideStrSize] = 0;

    mResource->SetName(wideName.c_str());

    //======================
    // Create upload buffer
    //======================
    D3D12_RESOURCE_DESC textureUploadDesc = {};
    textureUploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    textureUploadDesc.Alignment = allocInfo.Alignment;
    textureUploadDesc.Width = allocInfo.SizeInBytes;
    textureUploadDesc.Height = 1;
    textureUploadDesc.DepthOrArraySize = 1;
    textureUploadDesc.MipLevels = 1;
    textureUploadDesc.Format = DXGI_FORMAT_UNKNOWN;
    textureUploadDesc.SampleDesc.Count = 1;
    textureUploadDesc.SampleDesc.Quality = 0;
    textureUploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    textureUploadDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    uploadHeapProps.CreationNodeMask = 0;
    uploadHeapProps.VisibleNodeMask = 0;

    ComPtr<ID3D12Resource> uploadResource;
    hr = deviceContext.getDevice()->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &textureUploadDesc,
                                                            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                            IID_PPV_ARGS(&uploadResource));
    if(FAILED(hr))
    {
        spdlog::critical("Failed to create texture upload resource.");
        return Error::FailedToCreateUploadResource;
    }

    wideName.append(L" (Upload)");
    uploadResource->SetName(wideName.c_str());

    const gpufmt::FormatInfo& formatInfo = gpufmt::formatInfo(params.format);

    //============================================
    // 3. Copy cpu texture data to upload buffer
    //============================================
    if(!buffer.empty())
    {
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = buffer.data();
        subresourceData.RowPitch = buffer.size_bytes();
        subresourceData.SlicePitch = buffer.size_bytes();

        UpdateSubresources<1>(deviceContext.getCopyContext().getCommandList(), mResource.Get(), uploadResource.Get(), 0,
                              0, 1, &subresourceData);
    }

    deviceContext.getCopyContext().trackCopyResource(mResource, std::move(uploadResource));

    uint32_t descriptorCount = 0;
    const bool needsSrv = (params.accessFlags & ResourceAccessFlags::GpuRead) == ResourceAccessFlags::GpuRead;
    const bool needsUav = (params.accessFlags & ResourceAccessFlags::GpuWrite) == ResourceAccessFlags::GpuWrite;

    if(needsSrv) { ++descriptorCount; }
    if(needsUav) { ++descriptorCount; }

    auto descriptorHeapReservation = deviceContext.getCbvSrvUavHeap().reserve(descriptorCount);
    if(!descriptorHeapReservation) { return Error::InsufficientDescriptorHeapSpace; }

    mDescriptorHeapReservation = std::move(descriptorHeapReservation.value());

    uint32_t nextDescriptorIndex = 0;

    if(needsSrv)
    {
        mSrvIndex = nextDescriptorIndex++;
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = params.dxgiFormat;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = params.numElements;
        srvDesc.Buffer.StructureByteStride = (params.type == Type::Structured) ? params.elementByteSize : 0;
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        deviceContext.getCbvSrvUavHeap().createShaderResourceView(deviceContext, mDescriptorHeapReservation, mSrvIndex,
                                                                  mResource.Get(), srvDesc);
    }

    if(needsUav)
    {
        mUavIndex = nextDescriptorIndex++;
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = params.dxgiFormat;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = params.numElements;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.StructureByteStride = (params.type == Type::Structured) ? params.elementByteSize : 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        deviceContext.getCbvSrvUavHeap().createUnorderedAccessView(deviceContext, mDescriptorHeapReservation, mUavIndex,
                                                                   mResource.Get(), nullptr, uavDesc);
    }

    mUploadFrameCode = deviceContext.getCopyContext().getCurrentFrameCode();
    mParams = params;

    return std::nullopt;
}
} // namespace scrap::d3d12