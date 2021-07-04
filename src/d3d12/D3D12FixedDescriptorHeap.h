// Classes:
//   scrap::d3d12::FixedDescriptorHeapAllocator
//   scrap::d3d12::FixedDescriptorHeapAllocation
//   scrap::d3d12::FixedDescriptorHeapDescriptor
//   scrap::d3d12::FixedDescriptorHeap_CBV_SRV_UAV
//   scrap::d3d12::FixedDescriptorHeap_Sampler
//   scrap::d3d12::FixedDescriptorHeap_RTV
//   scrap::d3d12::FixedDescriptorHeap_DSV
//
// This collection of classes manage allocations into d3d12 descriptor heaps. The FixedDescriptorHeapAllocator starts
// with an fixed number of descriptors and will never grow. If the descriptor heap is fully allocated, all subsequent
// allocations will fail until some space is freed. All allocations and deallocations are cpu thread safe, but do not
// manage gpu lifetimes. Whatever code manages a FixedDescriptorHeapAllocator allocation MUST keep that allocation until
// the gpu has completely finished usin the associated descriptor.
//
// A fixed descriptor heap is useful for managing resources that have limited lifetimes, like textures and vertex
// buffers.

#pragma once

#include "FreeBlockTracker.h"

#include <array>
#include <cstdint>
#include <mutex>
#include <vector>

#include <tl/expected.hpp>
#include <wrl/client.h>

struct D3D12_CONSTANT_BUFFER_VIEW_DESC;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_DEPTH_STENCIL_VIEW_DESC;
struct D3D12_DESCRIPTOR_HEAP_DESC;
struct D3D12_GPU_DESCRIPTOR_HANDLE;
struct D3D12_RENDER_TARGET_VIEW_DESC;
struct D3D12_SAMPLER_DESC;
struct D3D12_SHADER_RESOURCE_VIEW_DESC;
struct D3D12_UNORDERED_ACCESS_VIEW_DESC;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;

enum D3D12_DESCRIPTOR_HEAP_TYPE;

namespace scrap::d3d12
{
class DeviceContext;

class FixedDescriptorHeapAllocation;

// This is the main class that manages allocations through a fixed sized descriptor heap. It allocates blocks of
// descriptors wrapped in the FixedDescriptorHeapAllocation class.
class FixedDescriptorHeapAllocator
{
public:
    FixedDescriptorHeapAllocator() = default;
    FixedDescriptorHeapAllocator(DeviceContext& context, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t descriptorCount);
    FixedDescriptorHeapAllocator(const FixedDescriptorHeapAllocator&) = delete;
    FixedDescriptorHeapAllocator(FixedDescriptorHeapAllocator&&) noexcept = default;
    virtual ~FixedDescriptorHeapAllocator();

    FixedDescriptorHeapAllocator& operator=(const FixedDescriptorHeapAllocator&) = delete;
    FixedDescriptorHeapAllocator& operator=(FixedDescriptorHeapAllocator&&) noexcept = default;

    [[nodiscard]] bool isValid() const { return mCpuHeap; }

    [[nodiscard]] ID3D12DescriptorHeap* getCpuDescriptorHeap() { return mCpuHeap.Get(); }
    [[nodiscard]] ID3D12DescriptorHeap* getGpuDescriptorHeap() { return mGpuHeap.Get(); }
    [[nodiscard]] uint32_t getDescriptorSize() const { return mDescriptorSize; }
    [[nodiscard]] uint32_t getDesctiptorCount() const { return static_cast<uint32_t>(mFreeBlockTracker.getCapacity()); }
    [[nodiscard]] size_t getByteSize() const
    {
        return static_cast<size_t>(mDescriptorSize) * mFreeBlockTracker.getCapacity() * 2;
    }

    [[nodiscard]] tl::expected<FixedDescriptorHeapAllocation, FreeBlockTracker::Error> allocate(
        uint32_t descriptorCount);

    void deallocate(FreeBlockTracker::Range range);

    // should happen at the beginning or end of a frame
    void uploadPendingDescriptors(DeviceContext& context);

protected:
    // Using two different heaps to help with upload performance. There's a flag for descriptor heaps,
    // D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, which indicates to the driver what type of gpu memory to allocate the
    // heap into. If a heap is shader visible, it will be put into memory that is fast for the gpu to read but probably
    // slow for communication with the cpu. Conversely, if that flag is not specified, the descriptor heap will be
    // placed in memory that is slower for gpu reads but has faster communication with the cpu. To take advantage of
    // this, descriptors are initially created in the cpu heap and copied to gpu heap.
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCpuHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mGpuHeap;
    std::mutex mMutex;
    FreeBlockTracker mFreeBlockTracker;
    std::vector<FreeBlockTracker::Range> mCpuRangesToCopy;
    uint32_t mDescriptorSize = 0u;
};

// Represents a contiguous block of allocated descriptors from a FixedDescriptorHeapAllocator. The allocation block held
// by this class can be further subdivided as need by other code. FixedDescriptorHeapAllocation will deallocate it's
// allocation block from the FixedDescriptorHeapAllocator it orginally allocated from upon destruction.
class FixedDescriptorHeapAllocation
{
public:
    FixedDescriptorHeapAllocation() = default;
    FixedDescriptorHeapAllocation(FixedDescriptorHeapAllocator& fixedHeap, FreeBlockTracker::Range range);
    FixedDescriptorHeapAllocation(const FixedDescriptorHeapAllocation&) = delete;
    FixedDescriptorHeapAllocation(FixedDescriptorHeapAllocation&&) noexcept;
    ~FixedDescriptorHeapAllocation();

    FixedDescriptorHeapAllocation& operator=(const FixedDescriptorHeapAllocation&) = delete;
    FixedDescriptorHeapAllocation& operator=(FixedDescriptorHeapAllocation&&) noexcept;

    [[nodiscard]] bool isValid() const { return mHeap != nullptr && mReservedHeapBlocks.count > 0; }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(uint32_t index) const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(uint32_t index) const;

    tl::expected<uint32_t, FreeBlockTracker::Error> reserveUnsynchronized();
    tl::expected<uint32_t, FreeBlockTracker::Error> reserve();
    void releaseUnsynchronized(uint32_t index);
    void release(uint32_t index);

private:
    FixedDescriptorHeapAllocator* mHeap = nullptr;
    FreeBlockTracker::Range mReservedHeapBlocks;
    FreeBlockTracker mSubAllocationTracker;
    std::mutex mMutex;
};

// Represents a single descriptor allocation from a FixedDescriptorHeapAllocator
class FixedDescriptorHeapDescriptor
{
public:
    FixedDescriptorHeapDescriptor() = default;
    FixedDescriptorHeapDescriptor(FixedDescriptorHeapAllocation&& allocation): mAllocation(std::move(allocation)) {}

    FixedDescriptorHeapDescriptor(const FixedDescriptorHeapDescriptor&) = delete;
    FixedDescriptorHeapDescriptor(FixedDescriptorHeapDescriptor&&) noexcept = default;
    ~FixedDescriptorHeapDescriptor() = default;

    FixedDescriptorHeapDescriptor& operator=(const FixedDescriptorHeapDescriptor&) = delete;
    FixedDescriptorHeapDescriptor& operator=(FixedDescriptorHeapDescriptor&&) noexcept = default;

    [[nodiscard]] bool isValid() const { return mAllocation.isValid(); }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle() const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle() const;

private:
    FixedDescriptorHeapAllocation mAllocation;
};

// Fixed descriptor heap allocator for constant buffer views, shader resource views, and unordered access views.
class FixedDescriptorHeap_CBV_SRV_UAV final : public FixedDescriptorHeapAllocator
{
public:
    FixedDescriptorHeap_CBV_SRV_UAV() = default;
    FixedDescriptorHeap_CBV_SRV_UAV(DeviceContext& context, uint32_t descriptorCount);
    FixedDescriptorHeap_CBV_SRV_UAV(const FixedDescriptorHeap_CBV_SRV_UAV&) = delete;
    FixedDescriptorHeap_CBV_SRV_UAV(FixedDescriptorHeap_CBV_SRV_UAV&&) noexcept = default;
    ~FixedDescriptorHeap_CBV_SRV_UAV() final = default;

    FixedDescriptorHeap_CBV_SRV_UAV& operator=(const FixedDescriptorHeap_CBV_SRV_UAV&) = delete;
    FixedDescriptorHeap_CBV_SRV_UAV& operator=(FixedDescriptorHeap_CBV_SRV_UAV&&) noexcept = default;

    // CBV
    void createConstantBufferView(DeviceContext& context,
                                  const FixedDescriptorHeapAllocation& allocation,
                                  uint32_t allocationIndex,
                                  const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error> createConstantBufferView(
        DeviceContext& context,
        const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    // SRV
    void createShaderResourceView(DeviceContext& context,
                                  const FixedDescriptorHeapAllocation& allocation,
                                  uint32_t allocationIndex,
                                  ID3D12Resource* resource,
                                  const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error> createShaderResourceView(
        DeviceContext& context,
        ID3D12Resource* resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    // UAV
    void createUnorderedAccessView(DeviceContext& context,
                                   const FixedDescriptorHeapAllocation& allocation,
                                   uint32_t allocationIndex,
                                   ID3D12Resource* resource,
                                   ID3D12Resource* counterResource,
                                   const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error> createUnorderedAccessView(
        DeviceContext& context,
        ID3D12Resource* resource,
        ID3D12Resource* counterResource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);
};

// Fixed descriptor heap for samplers.
class FixedDescriptorHeap_Sampler final : public FixedDescriptorHeapAllocator
{
public:
    FixedDescriptorHeap_Sampler() = default;
    FixedDescriptorHeap_Sampler(DeviceContext& context, uint32_t descriptorCount);
    FixedDescriptorHeap_Sampler(const FixedDescriptorHeap_Sampler&) = delete;
    FixedDescriptorHeap_Sampler(FixedDescriptorHeap_Sampler&&) noexcept = default;
    ~FixedDescriptorHeap_Sampler() final = default;

    FixedDescriptorHeap_Sampler& operator=(const FixedDescriptorHeap_Sampler&) = delete;
    FixedDescriptorHeap_Sampler& operator=(FixedDescriptorHeap_Sampler&&) noexcept = default;

    void createSampler(DeviceContext& context,
                       const FixedDescriptorHeapAllocation& allocation,
                       uint32_t allocationIndex,
                       const D3D12_SAMPLER_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error> createSampler(
        DeviceContext& context,
        const D3D12_SAMPLER_DESC& desc);
};

// Fixed descriptor heap for render target views.
class FixedDescriptorHeap_RTV final : public FixedDescriptorHeapAllocator
{
public:
    FixedDescriptorHeap_RTV() = default;
    FixedDescriptorHeap_RTV(DeviceContext& context, uint32_t descriptorCount);
    FixedDescriptorHeap_RTV(const FixedDescriptorHeap_RTV&) = delete;
    FixedDescriptorHeap_RTV(FixedDescriptorHeap_RTV&&) noexcept = default;
    ~FixedDescriptorHeap_RTV() final = default;

    FixedDescriptorHeap_RTV& operator=(const FixedDescriptorHeap_RTV&) = delete;
    FixedDescriptorHeap_RTV& operator=(FixedDescriptorHeap_RTV&&) noexcept = default;

    void createRenderTargetView(DeviceContext& context,
                                const FixedDescriptorHeapAllocation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* renderTargetResource,
                                const D3D12_RENDER_TARGET_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error> createRenderTargetView(
        DeviceContext& context,
        ID3D12Resource* renderTargetResource,
        const D3D12_RENDER_TARGET_VIEW_DESC& desc);
};

// Fixed descriptor heap for depth stencil views.
class FixedDescriptorHeap_DSV final : public FixedDescriptorHeapAllocator
{
public:
    FixedDescriptorHeap_DSV() = default;
    FixedDescriptorHeap_DSV(DeviceContext& context, uint32_t descriptorCount);
    FixedDescriptorHeap_DSV(const FixedDescriptorHeap_DSV&) = delete;
    FixedDescriptorHeap_DSV(FixedDescriptorHeap_DSV&&) noexcept = default;
    ~FixedDescriptorHeap_DSV() final = default;

    FixedDescriptorHeap_DSV& operator=(const FixedDescriptorHeap_DSV&) = delete;
    FixedDescriptorHeap_DSV& operator=(FixedDescriptorHeap_DSV&&) noexcept = default;

    void createDepthStencilView(DeviceContext& context,
                                const FixedDescriptorHeapAllocation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* depthStencilResource,
                                const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error> createDepthStencilView(
        DeviceContext& context,
        ID3D12Resource* depthStencilResource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
};

} // namespace scrap::d3d12