#pragma once

#include "Utility.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"

#include <array>

#include <cputex/unique_texture.h>
#include <d3d12.h>
#include <tl/expected.hpp>
#include <wrl/client.h>

namespace scrap::d3d12
{
class DeviceContext;

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

    enum class AccessFlags
    {
        CpuRead = 1 << 0,
        CpuWrite = 1 << 1,
        GpuRead = 1 << 2,
        GpuWrite = 1 << 3,
    };

    struct Params
    {
        cputex::TextureDimension dimension;
        gpufmt::Format format;
        cputex::Extent extents;
        uint32_t arraySize;
        uint8_t mipCount;
        AccessFlags accessFlags;
        std::string_view name;
    };

    Texture() = default;

    std::optional<Error>
    initUninitialized(DeviceContext& context, ID3D12GraphicsCommandList* commandList, const Params& params);
    std::optional<Error> initFromMemory(DeviceContext& context,
                                        ID3D12GraphicsCommandList* commandList,
                                        const cputex::TextureView& texture,
                                        AccessFlags accessFlags,
                                        std::string_view name);

    ID3D12Resource* getResource() const { return mResource.Get(); }

    D3D12_CPU_DESCRIPTOR_HANDLE getSrvCpu() const { return mDescriptorHeapReservation.getCpuHandle(mSrvIndex); }
    D3D12_GPU_DESCRIPTOR_HANDLE getSrvGpu() const { return mDescriptorHeapReservation.getGpuHandle(mSrvIndex); }

private:
    std::optional<Error> init(DeviceContext& context,
                              ID3D12GraphicsCommandList* commandList,
                              const Params& params,
                              const cputex::TextureView* texture);

    // Q: Why use two different GPU resource for the texture?
    // A: GPUs have different kinds of memory that are made faster for certain tasks but are slower for others. The
    // "upload" memory if more efficient at reading from cpu memory, but has less bandwidth available for the gpu
    // processor. The memory where the texture eventually ends up can't read from the cpu, and has full bandwidth to
    // the gpu processor. This type of memory is most of the memory on the gpu.
    // For more information read https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_type.
    // Specifically look at the descriptions for 'D3D12_HEAP_TYPE_UPLOAD' and 'D3D12_HEAP_TYPE_DEFAULT'.
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadResource;
    FixedDescriptorHeapReservation mDescriptorHeapReservation;
    uint32_t mSrvIndex = 0;
    uint32_t mUavIndex = 0;
    std::array<uint32_t, 15> mSrvMipIndices{0};
    std::array<uint32_t, 15> mUavMipIndices{0};
};

DEFINE_ENUM_BITWISE_OPERATORS(Texture::AccessFlags);

} // namespace scrap::d3d12