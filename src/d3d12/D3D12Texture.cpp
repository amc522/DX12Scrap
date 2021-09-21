#include "D3D12Texture.h"

#include "D3D12Context.h"
#include "D3D12Translations.h"

#include <ostream>

#include <cputex/utility.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <gpufmt/dxgi.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
std::optional<Texture::Error> Texture::initUninitialized(const Params& params)
{
    return init(params, nullptr);
}

std::optional<Texture::Error>
Texture::initFromMemory(const cputex::TextureView& texture, ResourceAccessFlags accessFlags, std::string_view name)
{
    Params params;
    params.dimension = texture.dimension();
    params.format = texture.format();
    params.extents = texture.extent();
    params.arraySize = texture.arraySize();
    params.mipCount = texture.mips();
    params.accessFlags = accessFlags;
    params.name = name;
    return init(params, &texture);
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::getSrvCpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getCpuHandle(mSrvIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::getSrvGpu() const
{
    return mResource.getCbvSrvUavDescriptorHeapReservation().getGpuHandle(mSrvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::getRtvCpu() const
{
    return mResource.getRtvDescriptorHeapReservation().getCpuHandle(mRtvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::getDsvCpu() const
{
    return mResource.getDsvDescriptorHeapReservation().getCpuHandle(mDsvIndex);
}

bool Texture::isReady() const
{
    return mResource != nullptr && mInitFrameCode < DeviceContext::instance().getCopyContext().getCurrentFrameCode();
}

void Texture::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    mResource.markAsUsed(commandQueue);
}

void Texture::markAsUsed(ID3D12CommandList* commandList)
{
    mResource.markAsUsed(commandList);
}

std::optional<Texture::Error> Texture::init(const Params& params, const cputex::TextureView* texture)
{
    DeviceContext& deviceContext = DeviceContext::instance();
    ID3D12GraphicsCommandList* commandList = deviceContext.getCopyContext().getCommandList();

    const auto formatTranslation = gpufmt::dxgi::translateFormat(params.format);

    if(!formatTranslation || !formatTranslation.exact) { return Error::InvalidFormat; }

    const gpufmt::FormatInfo& formatInfo = gpufmt::formatInfo(params.format);

    const bool isDepthStencil = formatInfo.depth || formatInfo.stencil;

    // VALIDATION
    if(params.isRenderTarget && isDepthStencil) { return Error::CannotUseDepthStencilFormatForRenderTarget; }

    if(params.isRenderTarget && !deviceContext.isSupportedRenderTargetFormat(formatTranslation.exact.value()))
    {
        return Error::InvalidRenderTargetFormat;
    }

    if(isDepthStencil && !deviceContext.isSupportedDepthStencilFormat(formatTranslation.exact.value()))
    {
        return Error::InvalidDepthStencilFormat;
    }

    if(isDepthStencil && ((params.accessFlags & ResourceAccessFlags::GpuWrite) == ResourceAccessFlags::GpuWrite))
    {
        return Error::UavNotSupportForDepthStencil;
    }

    // Setup formats
    const bool needsSrv = (params.accessFlags & ResourceAccessFlags::GpuRead) == ResourceAccessFlags::GpuRead;
    const bool needsUav = (params.accessFlags & ResourceAccessFlags::GpuWrite) == ResourceAccessFlags::GpuWrite;
    const bool needsRtv = params.isRenderTarget;
    const bool needsDsv = isDepthStencil;

    DXGI_FORMAT textureFormat = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT srvFormatPlane0 = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT srvFormatPlane1 = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT uavFormatPlane0 = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT uavFormatPlane1 = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;

    if(isDepthStencil)
    {
        const auto depthFormat = gpufmt::dxgi::depthShaderResourceViewFormat(formatTranslation.exact.value());
        if(!depthFormat.has_value()) { return Error::NoValidDepthSrvFormat; }

        srvFormatPlane0 = depthFormat.value();

        const auto stencilFormat = gpufmt::dxgi::stencilShaderResourceViewFormat(formatTranslation.exact.value());
        if(!stencilFormat.has_value()) { return Error::NoValidStencilSrvFormat; }

        srvFormatPlane1 = stencilFormat.value();

        dsvFormat = formatTranslation.exact.value();

        const auto typelessFormat = gpufmt::dxgi::typelessFormat(formatTranslation.exact.value());
        if(!typelessFormat.has_value()) { return Error::NoValidDepthStencilTypelessFormat; }

        textureFormat = typelessFormat.value();
    }
    else
    {
        auto typelessFormat = gpufmt::dxgi::typelessFormat(formatTranslation.exact.value());

        if(typelessFormat.has_value()) { textureFormat = typelessFormat.value(); }
        else
        {
            textureFormat = formatTranslation.exact.value();
        }

        srvFormatPlane0 = formatTranslation.exact.value();
        uavFormatPlane0 = formatTranslation.exact.value();
        rtvFormat = formatTranslation.exact.value();
    }

    HRESULT hr;

    // 1. Create GPU texture
    //    a. Get allocation info. This will get the byte aligment for the texture. You can set the alignment in
    //       D3D12_RESOURCE_DESC to 0 and it will figure out the alignment. Expicitly grabbing it here to use later.
    // 2. Create upload buffer
    // 3. Copy cpu texture data to upload buffer
    //    a. Query for the actual hardware storage information of the texture (D3D12_PLACED_SUBRESOURCE_FOOTPRINT
    //       via GetCopyableFootprints)
    //    b. Acquire pointer to upload texture memory (via Map)
    //    c. Copy cpu texture data to mapped upload buffer data
    //    d. Signal to the upload texture that the memory copy is finished (via Unmap)
    // 4. Copy the upload buffer to the final GPU texture (via CopyTextureRegion)
    // 5. Transition the resource from the copy state to the shader resource state
    // 6. Create the SRV

    //-----------------------------------------------------------------------------------
    // 1.a.
    // Get the alignment and size of the resource that will actually be used on the gpu.
    // ----------------------------------------------------------------------------------
    D3D12_RESOURCE_ALLOCATION_INFO allocInfo;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = TranslateResourceDimension(params.dimension);
    textureDesc.Alignment = 0;
    textureDesc.Width = params.extents.x;
    textureDesc.Height = params.extents.y;
    textureDesc.DepthOrArraySize =
        (params.dimension == cputex::TextureDimension::Texture3D) ? params.extents.z : params.arraySize;
    textureDesc.MipLevels = params.mipCount;
    textureDesc.Format = textureFormat;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    if((params.accessFlags & ResourceAccessFlags::GpuRead) != ResourceAccessFlags::GpuRead)
    {
        textureDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    }
    if((params.accessFlags & ResourceAccessFlags::GpuWrite) == ResourceAccessFlags::GpuWrite)
    {
        textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }
    if(isDepthStencil) { textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; }
    if(params.isRenderTarget) { textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; }

    // Here's and article describing in detail what this memory alignment stuff is all about:
    // https://asawicki.info/news_1726_secrets_of_direct3d_12_resource_alignment

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-getresourceallocationinfo
    allocInfo = deviceContext.getDevice()->GetResourceAllocationInfo(0, 1, &textureDesc);
    textureDesc.Alignment = allocInfo.Alignment;

    // The destination heap of the texture. This is the heap where the texture will be for its lifetime on the
    // gpu.
    D3D12_HEAP_PROPERTIES defaultHeapProps = {};
    defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    defaultHeapProps.CreationNodeMask = 0;
    defaultHeapProps.VisibleNodeMask = 0;

    // Maybe use D3D12_RESOURCE_STATE_COMMON when not copying the texture???
    D3D12_RESOURCE_STATES initialResourceState;
    if(texture != nullptr) { initialResourceState = D3D12_RESOURCE_STATE_COPY_DEST; }
    else if(params.isRenderTarget)
    {
        initialResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    else if(isDepthStencil)
    {
        initialResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }
    else
    {
        initialResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }

    {
        D3D12_CLEAR_VALUE clearValue;

        D3D12_CLEAR_VALUE* clearValuePtr = nullptr;
        if(params.isRenderTarget)
        {
            clearValue.Format = rtvFormat;
            std::memcpy(clearValue.Color, &params.colorClearValue[0], sizeof(clearValue.Color));
            clearValuePtr = &clearValue;
        }
        else if(isDepthStencil)
        {
            clearValue.Format = dsvFormat;
            clearValue.DepthStencil.Depth = params.depthClearValue;
            clearValue.DepthStencil.Stencil = params.stencilClearValue;
            clearValuePtr = &clearValue;
        }

        ComPtr<ID3D12Resource> resource;
        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommittedresource
        hr = deviceContext.getDevice()->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &textureDesc,
                                                                initialResourceState, clearValuePtr,
                                                                IID_PPV_ARGS(&resource));

        if(FAILED(hr)) { return Error::FailedToCreateResource; }

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

    //==========================
    // 2. Create upload buffer
    //==========================
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

    {
        ComPtr<ID3D12Resource> uploadResource;
        hr = deviceContext.getDevice()->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE,
                                                                &textureUploadDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                                nullptr, IID_PPV_ARGS(&uploadResource));
        if(FAILED(hr))
        {
            spdlog::critical("Failed to create texture upload resource.");
            return Error::FailedToCreateUploadResource;
        }

        mUploadResource = TrackedGpuObject(std::move(uploadResource));
    }

    wideName.append(L" (Upload)");
    mUploadResource->SetName(wideName.c_str());

    //============================================
    // 3. Copy cpu texture data to upload buffer
    //============================================
    if(texture != nullptr)
    {
        // Using 15 because its the maximum number of mips a 16384x16384 texture can have plus 1.
        std::array<D3D12_SUBRESOURCE_DATA, 16> subresources;
        UINT subresourceCount = 0;
        UINT firstSubresource = 0;

        for(uint32_t arraySlice = 0; arraySlice < texture->arraySize(); ++arraySlice)
        {
            for(uint32_t face = 0; face < texture->faces(); ++face)
            {
                for(uint32_t mip = 0; mip < texture->mips(); ++mip)
                {
                    // putting this assert here to check that surfaces are being access in order of incrementing d3d12
                    // subresource index
                    assert(arraySlice * texture->faces() * texture->mips() + face * texture->mips() + mip ==
                           D3D12CalcSubresource(mip, arraySlice * texture->faces() + face, 0, texture->mips(),
                                                texture->arraySize() * texture->faces()));

                    cputex::SurfaceView surface = texture->getMipSurface(arraySlice, face, mip);

                    const gpufmt::FormatInfo& formatInfo = gpufmt::formatInfo(surface.format());

                    D3D12_SUBRESOURCE_DATA& subresourceData = subresources[subresourceCount++];
                    subresourceData.pData = surface.getData().data();
                    subresourceData.RowPitch =
                        formatInfo.blockByteSize * (surface.extent().x / formatInfo.blockExtent.x);
                    subresourceData.SlicePitch = surface.sizeInBytes();

                    if(subresourceCount == subresources.size())
                    {
                        UpdateSubresources<subresources.size()>(commandList, mResource.getResource(),
                                                                mUploadResource.get(), 0, firstSubresource,
                                                                subresourceCount, subresources.data());
                        firstSubresource = subresourceCount;
                        subresourceCount = 0;
                    }
                }
            }
        }

        if(subresourceCount != 0)
        {
            UpdateSubresources<subresources.size()>(commandList, mResource.getResource(), mUploadResource.get(), 0,
                                                    firstSubresource, subresourceCount, subresources.data());
        }

        mResource.markAsUsed(commandList);
        mUploadResource.markAsUsed(commandList);
        mInitFrameCode = deviceContext.getCopyContext().getCurrentFrameCode();
    }

    if((params.accessFlags & ResourceAccessFlags::CpuWrite) != ResourceAccessFlags::CpuWrite)
    {
        // The upload buffer was only needed to initialize the resource and will not be used again.
        mUploadResource.reset();
    }

    uint32_t cbvSrvUavdescriptorCount = 0;
    uint32_t rtvDescriptorCount = 0;
    uint32_t dsvDescriptorCount = 0;
    if(needsSrv) { ++cbvSrvUavdescriptorCount; }
    if(needsUav) { ++cbvSrvUavdescriptorCount; }
    if(needsRtv) { ++rtvDescriptorCount; }
    if(needsDsv) { ++dsvDescriptorCount; }

    if(cbvSrvUavdescriptorCount > 0)
    {
        auto descriptorHeapReservation = deviceContext.getCbvSrvUavHeap().reserve(cbvSrvUavdescriptorCount);
        if(!descriptorHeapReservation) { return Texture::Error::InsufficientDescriptorHeapSpace; }

        mResource.setCbvSrvUavDescriptorHeapReservation(std::move(descriptorHeapReservation.value()));

        uint32_t nextCbvSrvUavDescriptorIndex = 0;

        if(needsSrv)
        {
            mSrvIndex = nextCbvSrvUavDescriptorIndex++;

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
            srvDesc.Format = srvFormatPlane0;
            srvDesc.ViewDimension = TranslateSrvDimension(params.dimension, params.arraySize);
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            switch(srvDesc.ViewDimension)
            {
            case D3D12_SRV_DIMENSION_TEXTURE1D:
                srvDesc.Texture1D.MostDetailedMip = 0;
                srvDesc.Texture1D.MipLevels = -1;
                srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
                break;
            case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
                srvDesc.Texture1DArray.MostDetailedMip = 0;
                srvDesc.Texture1DArray.MipLevels = -1;
                srvDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
                srvDesc.Texture1DArray.FirstArraySlice = 0;
                srvDesc.Texture1DArray.ArraySize = params.arraySize;
                break;
            case D3D12_SRV_DIMENSION_TEXTURE2D:
                srvDesc.Texture2D.MostDetailedMip = 0;
                srvDesc.Texture2D.MipLevels = -1;
                srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                srvDesc.Texture2D.PlaneSlice = 0;
                break;
            case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
                srvDesc.Texture2DArray.MostDetailedMip = 0;
                srvDesc.Texture2DArray.MipLevels = -1;
                srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
                srvDesc.Texture2DArray.PlaneSlice = 0;
                srvDesc.Texture2DArray.FirstArraySlice = 0;
                srvDesc.Texture2DArray.ArraySize = params.arraySize;
                break;
            case D3D12_SRV_DIMENSION_TEXTURECUBE:
                srvDesc.TextureCube.MostDetailedMip = 0;
                srvDesc.TextureCube.MipLevels = -1;
                srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
                break;
            case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
                srvDesc.TextureCubeArray.MostDetailedMip = 0;
                srvDesc.TextureCubeArray.MipLevels = -1;
                srvDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
                srvDesc.TextureCubeArray.First2DArrayFace = 0;
                srvDesc.TextureCubeArray.NumCubes = params.arraySize;
                break;
            case D3D12_SRV_DIMENSION_TEXTURE3D:
                srvDesc.Texture3D.MostDetailedMip = 0;
                srvDesc.Texture3D.MipLevels = -1;
                srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
                break;
            default: break;
            }

            deviceContext.getCbvSrvUavHeap().createShaderResourceView(deviceContext,
                                                                      mResource.getCbvSrvUavDescriptorHeapReservation(),
                                                                      mSrvIndex, mResource.getResource(), srvDesc);
        }

        if(needsUav)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
            desc.ViewDimension = TranslateUavDimension(params.dimension, params.arraySize);
            desc.Format = uavFormatPlane0;

            switch(desc.ViewDimension)
            {
            case D3D12_UAV_DIMENSION_TEXTURE1D: desc.Texture1D.MipSlice = 0; break;
            case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
                desc.Texture1DArray.MipSlice = 0;
                desc.Texture1DArray.ArraySize = params.arraySize;
                desc.Texture1DArray.FirstArraySlice = 0;
                break;
            case D3D12_UAV_DIMENSION_TEXTURE2D:
                desc.Texture2D.MipSlice = 0;
                desc.Texture2D.PlaneSlice = 0;
                break;
            case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
                desc.Texture2DArray.MipSlice = 0;
                desc.Texture2DArray.PlaneSlice = 0;
                desc.Texture2DArray.ArraySize = params.arraySize;
                desc.Texture2DArray.FirstArraySlice = 0;
                break;
            case D3D12_UAV_DIMENSION_TEXTURE3D:
                desc.Texture3D.MipSlice = 0;
                desc.Texture3D.WSize = params.extents.z;
                desc.Texture3D.FirstWSlice = 0;
                break;
            default: break;
            }

            deviceContext.getCbvSrvUavHeap().createUnorderedAccessView(
                deviceContext, mResource.getCbvSrvUavDescriptorHeapReservation(), mUavIndex, mResource.getResource(),
                nullptr, desc);
        }
    }

    if(needsRtv)
    {
        auto descriptorHeapReservation = deviceContext.getRtvHeap().reserve(rtvDescriptorCount);
        if(!descriptorHeapReservation) { return Texture::Error::InsufficientDescriptorHeapSpace; }

        mResource.setRtvDescriptorHeapReservation(std::move(descriptorHeapReservation.value()));

        mRtvIndex = 0;

        D3D12_RENDER_TARGET_VIEW_DESC desc;
        desc.ViewDimension = TranslateRtvDimension(params.dimension, params.arraySize, false);
        desc.Format = rtvFormat;

        switch(desc.ViewDimension)
        {
        case D3D12_RTV_DIMENSION_TEXTURE1D: desc.Texture1D.MipSlice = 0; break;
        case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
            desc.Texture1DArray.MipSlice = 0;
            desc.Texture1DArray.ArraySize = params.arraySize;
            desc.Texture1DArray.FirstArraySlice = 0;
            break;
        case D3D12_RTV_DIMENSION_TEXTURE2D:
            desc.Texture2D.MipSlice = 0;
            desc.Texture2D.PlaneSlice = 0;
            break;
        case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
            desc.Texture2DArray.MipSlice = 0;
            desc.Texture2DArray.ArraySize = params.arraySize;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.PlaneSlice = 0;
            break;
        case D3D12_RTV_DIMENSION_TEXTURE2DMS: break;
        case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
            desc.Texture2DMSArray.ArraySize = params.arraySize;
            desc.Texture2DMSArray.FirstArraySlice = 0;
            break;
        case D3D12_RTV_DIMENSION_TEXTURE3D:
            desc.Texture3D.MipSlice = 0;
            desc.Texture3D.WSize = params.extents.z;
            desc.Texture3D.FirstWSlice = 0;
            break;
        default: break;
        }

        deviceContext.getRtvHeap().createRenderTargetView(deviceContext, mResource.getRtvDescriptorHeapReservation(),
                                                          mRtvIndex, mResource.getResource(), desc);
    }

    if(needsDsv)
    {
        auto descriptorHeapReservation = deviceContext.getDsvHeap().reserve(dsvDescriptorCount);
        if(!descriptorHeapReservation) { return Texture::Error::InsufficientDescriptorHeapSpace; }

        mResource.setDsvDescriptorHeapReservation(std::move(descriptorHeapReservation.value()));

        mDsvIndex = 0;

        D3D12_DEPTH_STENCIL_VIEW_DESC desc;
        desc.ViewDimension = TranslateDsvDimension(params.dimension, params.arraySize, false);
        desc.Format = dsvFormat;
        desc.Flags = D3D12_DSV_FLAG_NONE;

        switch(desc.ViewDimension)
        {
        case D3D12_DSV_DIMENSION_TEXTURE1D: desc.Texture1D.MipSlice = 0; break;
        case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
            desc.Texture1DArray.MipSlice = 0;
            desc.Texture1DArray.ArraySize = params.arraySize;
            desc.Texture1DArray.FirstArraySlice = 0;
            break;
        case D3D12_DSV_DIMENSION_TEXTURE2D: desc.Texture2D.MipSlice = 0; break;
        case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
            desc.Texture2DArray.MipSlice = 0;
            desc.Texture2DArray.ArraySize = params.arraySize;
            desc.Texture2DArray.FirstArraySlice = 0;
            break;
        case D3D12_DSV_DIMENSION_TEXTURE2DMS: break;
        case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
            desc.Texture2DMSArray.ArraySize = params.arraySize;
            desc.Texture2DMSArray.FirstArraySlice = 0;
            break;
        default: break;
        }

        deviceContext.getDsvHeap().createDepthStencilView(deviceContext, mResource.getDsvDescriptorHeapReservation(),
                                                          mDsvIndex, mResource.getResource(), desc);
    }

    switch(params.dimension)
    {
    case cputex::TextureDimension::Texture1D:
        mResourceDimension =
            (params.arraySize > 1) ? ShaderResourceDimension::Texture1dArray : ShaderResourceDimension::Texture1d;
        break;
    case cputex::TextureDimension::Texture2D:
        mResourceDimension =
            (params.arraySize > 1) ? ShaderResourceDimension::Texture2dArray : ShaderResourceDimension::Texture2d;
        break;
    case cputex::TextureDimension::Texture3D: mResourceDimension = ShaderResourceDimension::Texture3d; break;
    case cputex::TextureDimension::TextureCube:
        mResourceDimension =
            (params.arraySize > 1) ? ShaderResourceDimension::TextureCubeArray : ShaderResourceDimension::TextureCube;
        break;
    default: mResourceDimension = ShaderResourceDimension::Unknown; break;
    }

    return std::nullopt;
}
} // namespace scrap::d3d12