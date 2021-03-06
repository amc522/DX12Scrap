#pragma once

#include "RenderDefs.h"
#include "StringUtils.h"
#include "Utility.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12FrameCodes.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <array>

#include <cputex/unique_texture.h>
#include <d3d12.h>
#include <tl/expected.hpp>
#include <wrl/client.h>

namespace scrap
{
namespace d3d12
{
enum class TextureError
{
    InvalidExtent,
    InvalidFormat,
    InvalidArraySize,
    NoValidDepthSrvFormat,
    NoValidStencilSrvFormat,
    NoValidDepthStencilTypelessFormat,
    UavNotSupportForDepthStencil,
    CannotUseDepthStencilFormatForRenderTarget,
    InvalidDepthStencilFormat,
    InvalidRenderTargetFormat,
    FailedToCreateResource,
    FailedToCreateUploadResource,
    InsufficientDescriptorHeapSpace,
};

struct TextureParams
{
    cputex::TextureDimension dimension;
    gpufmt::Format format;
    cputex::Extent extents;
    uint32_t arraySize;
    uint8_t mipCount;

    // accessFlags:
    // For render targets and depth stencil targets, GpuRead and GpuWrite do not need to be specified. If GpuRead is
    // specified, srvs are created for the render target or depth and stencil planes. For render targets, if
    // GpuWrite is specified, a uav will be created. GpuWrite is not valid for depth stencil targets.
    ResourceAccessFlags accessFlags;
    std::optional<D3D12_RESOURCE_STATES> initialResourceState;
    std::string_view name;
    glm::vec4 colorClearValue{0.0f, 0.0f, 0.0f, 1.0f};
    float depthClearValue = 0.0f;
    uint8_t stencilClearValue = 0;
    bool isRenderTarget = false;
};

class Texture
{
public:
    Texture() = default;
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    ~Texture() = default;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    std::optional<TextureError> initUninitialized(const TextureParams& params);
    std::optional<TextureError>
    initFromMemory(const cputex::TextureView& texture, ResourceAccessFlags accessFlags, std::string_view name);

    ID3D12Resource* getResource() const { return mResource.getResource(); }

    D3D12_CPU_DESCRIPTOR_HANDLE getSrvCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getSrvGpu() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getRtvCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getRtvGpu() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getDsvCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getDsvGpu() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getUavCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getUavGpu() const;

    uint32_t getSrvDescriptorHeapIndex() const
    {
        return mResource.getCbvSrvUavDescriptorHeapReservation().getStartHeapIndex() + mSrvIndex;
    }

    uint32_t getUavDescriptorHeapIndex() const
    {
        return mResource.getCbvSrvUavDescriptorHeapReservation().getStartHeapIndex() + mUavIndex;
    }

    ShaderResourceDimension getShaderResourceDimension() const { return mResourceDimension; }

    bool isReady() const;

    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

private:
    std::optional<TextureError> init(const TextureParams& params, const cputex::TextureView* texture);

    // Q: Why use two different GPU resource for the texture?
    // A: GPUs have different kinds of memory that are made faster for certain tasks but are slower for others. The
    // "upload" memory if more efficient at reading from cpu memory, but has less bandwidth available for the gpu
    // processor. The memory where the texture eventually ends up can't read from the cpu, and has full bandwidth to
    // the gpu processor. This type of memory is most of the memory on the gpu.
    // For more information read https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_type.
    // Specifically look at the descriptions for 'D3D12_HEAP_TYPE_UPLOAD' and 'D3D12_HEAP_TYPE_DEFAULT'.
    TrackedShaderResource mResource;
    TrackedGpuObject<ID3D12Resource> mUploadResource;
    uint32_t mSrvIndex = std::numeric_limits<uint32_t>::max();
    uint32_t mUavIndex = std::numeric_limits<uint32_t>::max();
    uint32_t mRtvIndex = std::numeric_limits<uint32_t>::max();
    uint32_t mDsvIndex = std::numeric_limits<uint32_t>::max();

    ShaderResourceDimension mResourceDimension = ShaderResourceDimension::Unknown;

    CopyFrameCode mInitFrameCode;
};
} // namespace d3d12

template<>
constexpr std::string_view ToStringView(d3d12::TextureError error)
{
    switch(error)
    {
    case d3d12::TextureError::InvalidExtent: return "InvalidExtent";
    case d3d12::TextureError::InvalidFormat: return "InvalidFormat";
    case d3d12::TextureError::InvalidArraySize: return "InvalidArraySize";
    case d3d12::TextureError::NoValidDepthSrvFormat: return "NoValidDepthSrvFormat";
    case d3d12::TextureError::NoValidStencilSrvFormat: return "NoValidStencilSrvFormat";
    case d3d12::TextureError::NoValidDepthStencilTypelessFormat: return "NoValidDepthStencilTypelessFormat";
    case d3d12::TextureError::UavNotSupportForDepthStencil: return "UavNotSupportForDepthStencil";
    case d3d12::TextureError::CannotUseDepthStencilFormatForRenderTarget:
        return "CannotUseDepthStencilFormatForRenderTarget";
    case d3d12::TextureError::InvalidDepthStencilFormat: return "InvalidDepthStencilFormat";
    case d3d12::TextureError::InvalidRenderTargetFormat: return "InvalidRenderTargetFormat";
    case d3d12::TextureError::FailedToCreateResource: return "FailedToCreateResource";
    case d3d12::TextureError::FailedToCreateUploadResource: return "FailedToCreateUploadResource";
    case d3d12::TextureError::InsufficientDescriptorHeapSpace: return "InsufficientDescriptorHeapSpace";
    default: return "Unknown TextureError";
    }
}

} // namespace scrap

template<>
struct fmt::formatter<scrap::d3d12::TextureError> : public scrap::ToStringViewFormatter<scrap::d3d12::TextureError>
{};