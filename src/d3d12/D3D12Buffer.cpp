#include "d3d12/D3D12Buffer.h"

#include "Utility.h"
#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <gpufmt/dxgi.h>
#include <gpufmt/traits.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
std::optional<BufferError> Buffer::init(const BufferSimpleParams& params, std::span<const std::byte> buffer)
{
    return initInternal(params, buffer);
}

std::optional<BufferError> Buffer::init(const BufferFormattedParams& params, std::span<const std::byte> buffer)
{
    return initInternal(params, buffer);
}

std::optional<BufferError> Buffer::init(const BufferStructuredParams& params, std::span<const std::byte> buffer)
{
    return initInternal(params, buffer);
}

D3D12_CPU_DESCRIPTOR_HANDLE Buffer::getSrvCpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getCpuHandle(mSrvIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE Buffer::getSrvGpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getGpuHandle(mSrvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE Buffer::getUavCpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getCpuHandle(mUavIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE Buffer::getUavGpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getGpuHandle(mUavIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE Buffer::getCbvCpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getCpuHandle(mCbvIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE Buffer::getCbvGpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getGpuHandle(mCbvIndex);
}

D3D12_INDEX_BUFFER_VIEW Buffer::getIndexView() const
{
    return getIndexView(mParams.dxgiFormat);
}

D3D12_INDEX_BUFFER_VIEW Buffer::getIndexView(DXGI_FORMAT format) const
{
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = mResource.getResource()->GetGPUVirtualAddress();
    ibv.Format = format;
    ibv.SizeInBytes = (uint32_t)mParams.byteSize;
    return ibv;
}

bool Buffer::isReady() const
{
    return mResource != nullptr && mInitFrameCode < DeviceContext::instance().getCopyContext().getCurrentFrameCode();
}

void Buffer::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    mResource.markAsUsed(commandQueue);
}

void Buffer::markAsUsed(ID3D12CommandList* commandList)
{
    mResource.markAsUsed(commandList);
}

std::span<std::byte> Buffer::map()
{
    void* data;
    if(FAILED(mUploadResource->Map(0, nullptr, &data))) { return {}; }

    return std::span<std::byte>(reinterpret_cast<std::byte*>(data), mParams.byteSize);
}

void Buffer::unmap(ID3D12GraphicsCommandList* commandList)
{
    mUploadResource->Unmap(0, nullptr);

    commandList->CopyBufferRegion(mResource.getResource(), 0, mUploadResource.get(), 0, mParams.byteSize);
    mResource.markAsUsed(commandList);
    mUploadResource.markAsUsed(commandList);
}

void Buffer::transition(ID3D12GraphicsCommandList* commandList,
                        D3D12_RESOURCE_STATES before,
                        D3D12_RESOURCE_STATES after)
{
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mResource.getResource(), before, after);
    commandList->ResourceBarrier(1, &barrier);
}

std::optional<BufferError> Buffer::initInternal(Params params, std::span<const std::byte> buffer)
{
    assert(!mInitialized);

    DeviceContext& deviceContext = DeviceContext::instance();
    HRESULT hr;

    const bool isConstantBuffer = (params.flags & BufferFlags::ConstantBuffer) == BufferFlags::ConstantBuffer;
    const bool isIndexBuffer = (params.flags & BufferFlags::IndexBuffer) == BufferFlags::IndexBuffer;
    const bool isAccelerationStructure =
        (params.flags & BufferFlags::AccelerationStructure) == BufferFlags::AccelerationStructure;

    if(params.type == Type::Formatted)
    {
        const auto formatTranslation = gpufmt::dxgi::translateFormat(params.format);
        if(!formatTranslation || !formatTranslation.exact) { return BufferError::InvalidFormat; }
        params.dxgiFormat = formatTranslation.exact.value();

        const auto& formatInfo = gpufmt::formatInfo(params.format);
        params.byteSize = (uint64_t)formatInfo.blockByteSize * (uint64_t)params.numElements;
        params.elementByteSize = formatInfo.blockByteSize;
    }
    else if(params.type == Type::Structured)
    {
        params.dxgiFormat = DXGI_FORMAT_UNKNOWN;
        params.byteSize = (uint64_t)params.numElements * (uint64_t)params.elementByteSize;
    }
    else if(isConstantBuffer)
    {
        // Got a d3d12 debug layer error when creating a constant buffer view that wasn't a multiple of 256. Don't know
        // if this is device specific or if it's queriable. Tested on an NVIDIA GeForce GTX 970M and an Intel HD
        // Graphics 530 and both reported the same 256 byte alignment requirement.
        //
        // D3D12 ERROR: ID3D12Device::CreateConstantBufferView: Size of 4 is invalid.  Device requires
        // SizeInBytes be a multiple of 256. [ STATE_CREATION ERROR #650: CREATE_CONSTANT_BUFFER_VIEW_INVALID_DESC]

        params.dxgiFormat = DXGI_FORMAT_UNKNOWN;
        params.byteSize = AlignInteger(params.byteSize, 256ull);
    }
    else if(isAccelerationStructure)
    {
        params.dxgiFormat = DXGI_FORMAT_UNKNOWN;
        params.byteSize =
            AlignInteger(params.byteSize, uint64_t(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT));
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

    if((params.accessFlags & ResourceAccessFlags::GpuRead) != ResourceAccessFlags::GpuRead &&
       (params.flags & BufferFlags::NonPixelShaderResource) != BufferFlags::NonPixelShaderResource &&
       !isAccelerationStructure && !isConstantBuffer)
    {
        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    }
    if((params.accessFlags & ResourceAccessFlags::GpuWrite) == ResourceAccessFlags::GpuWrite ||
       (params.flags & BufferFlags::UavEnabled) == BufferFlags::UavEnabled || isAccelerationStructure)
    {
        bufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    // Here's and article describing in detail what this memory alignment stuff is all about:
    // https://asawicki.info/news_1726_secrets_of_direct3d_12_resource_alignment

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-getresourceallocationinfo
    allocInfo = deviceContext.getDevice()->GetResourceAllocationInfo(0, 1, &bufferDesc);
    bufferDesc.Alignment = allocInfo.Alignment;

    D3D12_RESOURCE_STATES initialResourceState = params.initialResourceState.value_or(D3D12_RESOURCE_STATE_COMMON);
    D3D12_RESOURCE_STATES postCopyState = D3D12_RESOURCE_STATE_COMMON;

    if(!buffer.empty())
    {
        postCopyState = initialResourceState;

        // The initial resource state was originally D3D12_RESOURCE_STATE_COPY_DEST, but a D3D12 warning started popping
        // up.
        //
        // D3D12 WARNING: ID3D12Device::CreateCommittedResource2: Ignoring InitialState
        //                D3D12_RESOURCE_STATE_COPY_DEST. Buffers are effectively created in state
        //                D3D12_RESOURCE_STATE_COMMON. [STATE_CREATION WARNING #1328: CREATERESOURCE_STATE_IGNORED]
        initialResourceState = D3D12_RESOURCE_STATE_COMMON;
    }

    // The destination heap of the buffer. This is the heap where the buffer will be for its lifetime on the
    // gpu.
    D3D12_HEAP_PROPERTIES defaultHeapProps = {};
    defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    defaultHeapProps.CreationNodeMask = 0;
    defaultHeapProps.VisibleNodeMask = 0;

    {
        ComPtr<ID3D12Resource> resource;
        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommittedresource
        hr = deviceContext.getDevice()->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                                initialResourceState, nullptr, IID_PPV_ARGS(&resource));

        if(FAILED(hr)) { return BufferError::FailedToCreateGpuResource; }

        mResource = TrackedShaderResource(std::move(resource));
    }

    std::wstring wideName;
    int wideStrSize = MultiByteToWideChar(CP_UTF8, 0u, params.name.data(), (int)params.name.size(), nullptr, 0);
    wideName.reserve(wideStrSize + sizeof(" (Upload)"));
    wideName.resize(wideStrSize);
    MultiByteToWideChar(CP_UTF8, 0u, params.name.data(), (int)params.name.size(), wideName.data(),
                        (int)wideName.size());
    wideName.data()[wideStrSize] = 0;

    mResource.getResource()->SetName(wideName.c_str());

    //======================
    // Create upload buffer
    //======================
    D3D12_RESOURCE_DESC textureUploadDesc = {};
    textureUploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    textureUploadDesc.Alignment = allocInfo.Alignment;
    textureUploadDesc.Width = params.byteSize;
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

    {
        ComPtr<ID3D12Resource> uploadResource;
        hr = deviceContext.getDevice()->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE,
                                                                &textureUploadDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                                nullptr, IID_PPV_ARGS(&uploadResource));
        if(FAILED(hr))
        {
            spdlog::critical("Failed to create texture upload resource.");
            return BufferError::FailedToCreateUploadResource;
        }

        mUploadResource = TrackedGpuObject(std::move(uploadResource));
    }

    wideName.append(L" (Upload)");
    mUploadResource->SetName(wideName.c_str());

    const gpufmt::FormatInfo& formatInfo = gpufmt::formatInfo(params.format);

    //============================================
    // 3. Copy cpu buffer data to upload buffer
    //============================================
    if(!buffer.empty())
    {
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = buffer.data();
        subresourceData.RowPitch = buffer.size_bytes();
        subresourceData.SlicePitch = buffer.size_bytes();

        UpdateSubresources<1>(deviceContext.getCopyContext().getCommandList(), mResource.getResource(),
                              mUploadResource.get(), 0, 0, 1, &subresourceData);

        if(initialResourceState != postCopyState)
        {
            D3D12_RESOURCE_BARRIER barrier;
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = mResource.getResource();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = postCopyState;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

            deviceContext.getCopyContext().getCommandList()->ResourceBarrier(1, &barrier);
        }

        mResource.markAsUsed(deviceContext.getCopyContext().getCommandList());
        mUploadResource.markAsUsed(deviceContext.getCopyContext().getCommandList());
        mInitFrameCode = deviceContext.getCopyContext().getCurrentFrameCode();
    }

    if((params.accessFlags & ResourceAccessFlags::CpuWrite) != ResourceAccessFlags::CpuWrite)
    {
        // The upload buffer was only needed to initialize the resource and will not be used again.
        mUploadResource.reset();
    }

    uint32_t descriptorCount = 0;

    const bool needsSrv =
        (params.accessFlags & ResourceAccessFlags::GpuRead) == ResourceAccessFlags::GpuRead && !isConstantBuffer;
    const bool needsUav = (params.accessFlags & ResourceAccessFlags::GpuWrite) == ResourceAccessFlags::GpuWrite;
    const bool needsCbv = isConstantBuffer;

    if(needsSrv) { ++descriptorCount; }
    if(needsUav) { ++descriptorCount; }
    if(needsCbv) { ++descriptorCount; }

    auto descriptorHeapReservation = deviceContext.getCbvSrvUavHeap().reserve(descriptorCount);
    if(!descriptorHeapReservation) { return BufferError::InsufficientDescriptorHeapSpace; }

    mResource.setCbvSrvUavDescriptorHeapReservation(std::move(descriptorHeapReservation.value()));

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

        D3D12_BUFFER_SRV_FLAGS flags = D3D12_BUFFER_SRV_FLAG_NONE;
        if((params.flags & BufferFlags::Raw) == BufferFlags::Raw) { flags |= D3D12_BUFFER_SRV_FLAG_RAW; }
        srvDesc.Buffer.Flags = flags;

        deviceContext.getCbvSrvUavHeap().createShaderResourceView(deviceContext,
                                                                  mResource.getCbvSrvUavDescriptorHeapReservation(),
                                                                  mSrvIndex, mResource.getResource(), srvDesc);
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

        D3D12_BUFFER_UAV_FLAGS flags = D3D12_BUFFER_UAV_FLAG_NONE;
        if((params.flags & BufferFlags::Raw) == BufferFlags::Raw) { flags |= D3D12_BUFFER_UAV_FLAG_RAW; }
        uavDesc.Buffer.Flags = flags;

        deviceContext.getCbvSrvUavHeap().createUnorderedAccessView(
            deviceContext, mResource.getCbvSrvUavDescriptorHeapReservation(), mUavIndex, mResource.getResource(),
            nullptr, uavDesc);
    }

    if(needsCbv)
    {
        mCbvIndex = nextDescriptorIndex++;
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = mResource.getResource()->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = (uint32_t)params.byteSize;

        deviceContext.getCbvSrvUavHeap().createConstantBufferView(
            deviceContext, mResource.getCbvSrvUavDescriptorHeapReservation(), mCbvIndex, cbvDesc);
    }

    mParams = params;

    mInitialized = true;

    return std::nullopt;
}
} // namespace scrap::d3d12