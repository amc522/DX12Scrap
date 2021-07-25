#include "D3D12Texture.h"

#include "D3D12Context.h"

#include <ostream>

#include <cputex/utility.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <gpufmt/dxgi.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
D3D12_RESOURCE_DIMENSION TranslateTextureDimension(cputex::TextureDimension textureDimension)
{
    switch(textureDimension)
    {
    case cputex::TextureDimension::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    case cputex::TextureDimension::Texture2D: [[fallthrough]];
    case cputex::TextureDimension::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    case cputex::TextureDimension::TextureCube: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    default: return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }
}

D3D12_SRV_DIMENSION TranslateTextureDimensionToSrv(cputex::TextureDimension textureDimension, size_t arraySize)
{
    switch(textureDimension)
    {
    case cputex::TextureDimension::Texture1D:
        return (arraySize > 1) ? D3D12_SRV_DIMENSION_TEXTURE1DARRAY : D3D12_SRV_DIMENSION_TEXTURE1D;
    case cputex::TextureDimension::Texture2D:
        return (arraySize > 1) ? D3D12_SRV_DIMENSION_TEXTURE2DARRAY : D3D12_SRV_DIMENSION_TEXTURE2D;
    case cputex::TextureDimension::Texture3D: return D3D12_SRV_DIMENSION_TEXTURE3D;
    case cputex::TextureDimension::TextureCube:
        return (arraySize > 1) ? D3D12_SRV_DIMENSION_TEXTURECUBEARRAY : D3D12_SRV_DIMENSION_TEXTURECUBE;
    default: return D3D12_SRV_DIMENSION_UNKNOWN;
    }
}

Texture::~Texture()
{
    DeviceContext::instance().queueResourceForDestruction(std::move(mResource), std::move(mDescriptorHeapReservation), mLastUsedFrameCode);
}

std::optional<Texture::Error> Texture::initUninitialized(DeviceContext& context, const Params& params)
{
    return init(context, params, nullptr);
}

std::optional<Texture::Error> Texture::initFromMemory(DeviceContext& context,
                                                      const cputex::TextureView& texture,
                                                      ResourceAccessFlags accessFlags,
                                                      std::string_view name)
{
    Params params;
    params.dimension = texture.dimension();
    params.format = texture.format();
    params.extents = texture.extent();
    params.arraySize = texture.arraySize();
    params.mipCount = texture.mips();
    params.accessFlags = accessFlags;
    params.name = name;
    return init(context, params, &texture);
}

bool Texture::isReady() const
{
    return mResource != nullptr && mUploadFrameCode <= DeviceContext::instance().getCopyContext().getLastCompletedFrameCode();
}

void Texture::markAsUsed()
{
    mLastUsedFrameCode = DeviceContext::instance().getCurrentFrameCode();
}

std::optional<Texture::Error>
Texture::init(DeviceContext& context, const Params& params, const cputex::TextureView* texture)
{
    ID3D12GraphicsCommandList* commandList = context.getCopyContext().getCommandList();

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

    const auto formatTranslation = gpufmt::dxgi::translateFormat(params.format);

    if(!formatTranslation || !formatTranslation.exact) { return Error::InvalidFormat; }

    //-----------------------------------------------------------------------------------
    // 1.a.
    // Get the alignment and size of the resource that will actually be used on the gpu.
    // ----------------------------------------------------------------------------------
    D3D12_RESOURCE_ALLOCATION_INFO allocInfo;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = TranslateTextureDimension(params.dimension);
    textureDesc.Alignment = 0;
    textureDesc.Width = params.extents.x;
    textureDesc.Height = params.extents.y;
    textureDesc.DepthOrArraySize =
        (params.dimension == cputex::TextureDimension::Texture3D) ? params.extents.z : params.arraySize;
    textureDesc.MipLevels = params.mipCount;
    textureDesc.Format = formatTranslation.exact.value();
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

    // Here's and article describing in detail what this memory alignment stuff is all about:
    // https://asawicki.info/news_1726_secrets_of_direct3d_12_resource_alignment

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-getresourceallocationinfo
    allocInfo = context.getDevice()->GetResourceAllocationInfo(0, 1, &textureDesc);
    textureDesc.Alignment = allocInfo.Alignment;

    // The destination heap of the texture. This is the heap where the texture will be for its lifetime on the
    // gpu.
    D3D12_HEAP_PROPERTIES defaultHeapProps = {};
    defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    defaultHeapProps.CreationNodeMask = 0;
    defaultHeapProps.VisibleNodeMask = 0;

    // Maybe use D3D12_RESOURCE_STATE_COMMON when not copy the texture???
    D3D12_RESOURCE_STATES initialResourceState =
        (texture != nullptr) ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommittedresource
    HRESULT hr =
        context.getDevice()->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &textureDesc,
                                                     D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&mResource));

    if(FAILED(hr)) { return Error::FailedToCreateResource; }

    std::wstring wideName;
    int wideStrSize = MultiByteToWideChar(CP_UTF8, 0u, params.name.data(), (int)params.name.size(), nullptr, 0);
    wideName.reserve(wideStrSize + sizeof(" (Upload)"));
    wideName.resize(wideStrSize);
    MultiByteToWideChar(CP_UTF8, 0u, params.name.data(), (int)params.name.size(), wideName.data(),
                        (int)wideName.size());
    wideName.data()[wideStrSize] = 0;

    mResource->SetName(wideName.c_str());

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

    ComPtr<ID3D12Resource> uploadResource;
    hr = context.getDevice()->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &textureUploadDesc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                      IID_PPV_ARGS(&uploadResource));
    if(FAILED(hr))
    {
        spdlog::critical("Failed to create texture upload resource.");
        return Error::FailedToCreateUploadResource;
    }

    wideName.append(L" (Upload)");
    uploadResource->SetName(wideName.c_str());

    //============================================
    // 3. Copy cpu texture data to upload buffer
    //============================================
    if(texture != nullptr)
    {
        const gpufmt::FormatInfo& formatInfo = gpufmt::formatInfo(params.format);

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
                        UpdateSubresources<subresources.size()>(commandList, mResource.Get(), uploadResource.Get(), 0,
                                                                firstSubresource, subresourceCount,
                                                                subresources.data());
                        firstSubresource = subresourceCount;
                        subresourceCount = 0;
                    }
                }
            }
        }

        if(subresourceCount != 0)
        {
            UpdateSubresources<subresources.size()>(commandList, mResource.Get(), uploadResource.Get(), 0,
                                                    firstSubresource, subresourceCount, subresources.data());
        }
    }

    context.getCopyContext().trackCopyResource(mResource, std::move(uploadResource));

    auto descriptorHeapReservation = context.getCbvSrvUavHeap().reserve(1);
    if(!descriptorHeapReservation) { return Texture::Error::InsufficientDescriptorHeapSpace; }

    mDescriptorHeapReservation = std::move(descriptorHeapReservation.value());

    uint32_t nextDescriptorIndex = 0;
    mSrvIndex = nextDescriptorIndex++;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = formatTranslation.exact.value();
    srvDesc.ViewDimension = TranslateTextureDimensionToSrv(params.dimension, params.arraySize);
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

    context.getCbvSrvUavHeap().createShaderResourceView(context, mDescriptorHeapReservation, mSrvIndex, mResource.Get(),
                                                        srvDesc);

    mUploadFrameCode = context.getCopyContext().getCurrentFrameCode();

    return std::nullopt;
}
} // namespace scrap::d3d12