#pragma once

#include "RenderDefs.h"
#include "Utility.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12FrameCodes.h"

#include <array>

#include <cputex/unique_texture.h>
#include <tl/expected.hpp>
#include <wrl/client.h>

struct ID3D12Resource;

namespace scrap::d3d12
{
class Texture
{
public:
    enum class Error
    {
        InvalidExtent,
        InvalidFormat,
        InavlidArraySize,
        FailedToCreateResource,
        FailedToCreateUploadResource,
        InsufficientDescriptorHeapSpace,
    };

    struct Params
    {
        cputex::TextureDimension dimension;
        gpufmt::Format format;
        cputex::Extent extents;
        uint32_t arraySize;
        uint8_t mipCount;
        ResourceAccessFlags accessFlags;
        std::string_view name;
    };

    Texture() = default;
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    ~Texture();

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    std::optional<Error> initUninitialized(const Params& params);
    std::optional<Error>
    initFromMemory(const cputex::TextureView& texture, ResourceAccessFlags accessFlags, std::string_view name);

    ID3D12Resource* getResource() const { return mResource.Get(); }

    D3D12_CPU_DESCRIPTOR_HANDLE getSrvCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getSrvGpu() const;

    bool isReady() const;

    void markAsUsed();

private:
    std::optional<Error> init(const Params& params, const cputex::TextureView* texture);

    // Q: Why use two different GPU resource for the texture?
    // A: GPUs have different kinds of memory that are made faster for certain tasks but are slower for others. The
    // "upload" memory if more efficient at reading from cpu memory, but has less bandwidth available for the gpu
    // processor. The memory where the texture eventually ends up can't read from the cpu, and has full bandwidth to
    // the gpu processor. This type of memory is most of the memory on the gpu.
    // For more information read https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_type.
    // Specifically look at the descriptions for 'D3D12_HEAP_TYPE_UPLOAD' and 'D3D12_HEAP_TYPE_DEFAULT'.
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    FixedDescriptorHeapReservation mDescriptorHeapReservation;
    uint32_t mSrvIndex = 0;
    uint32_t mUavIndex = 0;
    std::array<uint32_t, 15> mSrvMipIndices{0};
    std::array<uint32_t, 15> mUavMipIndices{0};

    CopyFrameCode mUploadFrameCode;
    RenderFrameCode mLastUsedFrameCode;
};

} // namespace scrap::d3d12