// Classes:
//   scrap::d3d12::FixedDescriptorHeapAllocator
//   scrap::d3d12::FixedDescriptorHeapResevation
//   scrap::d3d12::FixedDescriptorHeapSubAllocator
//   scrap::d3d12::FixedDescriptorHeapMonotonicSubAllocator
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

class FixedDescriptorHeapReservation;
class FixedDescriptorHeapSubAllocator;
class FixedDescriptorHeapMonotonicSubAllocator;

// This is the main class that manages allocations through a fixed sized descriptor heap. It allocates ranges of the
// descriptor heap to be used by other object however they want. There's two types of allocations that can be made;
// normal and monotonic. The normal allocation can have each block allocated and deallocated as many times as needed.
// The monotonic allocation can only be allocated to and once all its blocks have been used, it can no longer allocate.
//
// An example use for the normal allocation would be for a system that needs to create vertex buffers every frame. Each
// frame that system would be creating new SRVs and possibly UAVs and then destroying them at the end of the frame.
//
// An example use for the monotonic allocation would be a texture object. The texture object might need to create
// several different views into the texture. Those views will be create all at once and destroyed when the texture is
// destroyed. No deallocation needs to happen on a per descriptor basis.
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

    [[nodiscard]] tl::expected<std::shared_ptr<FixedDescriptorHeapSubAllocator>, FreeBlockTracker::Error>
    allocateSubHeap(uint32_t descriptorCount);

    [[nodiscard]] tl::expected<std::shared_ptr<FixedDescriptorHeapMonotonicSubAllocator>, FreeBlockTracker::Error>
    allocateMonotonicSubHeap(uint32_t descriptorCount);

    [[nodiscard]] tl::expected<FixedDescriptorHeapReservation, FreeBlockTracker::Error>
    reserve(uint32_t descriptorCount);

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

// Represents a contiguous block of descriptors from a FixedDescriptorHeapAllocator. It does not keep track of any
// allocations within its range. On destruction it will deallocate the range of reserved descriptors.
class FixedDescriptorHeapReservation
{
public:
public:
    FixedDescriptorHeapReservation() = default;
    FixedDescriptorHeapReservation(FixedDescriptorHeapAllocator& fixedHeap, FreeBlockTracker::Range range);
    FixedDescriptorHeapReservation(const FixedDescriptorHeapReservation&) = delete;
    FixedDescriptorHeapReservation(FixedDescriptorHeapReservation&& other) noexcept;
    virtual ~FixedDescriptorHeapReservation();

    FixedDescriptorHeapReservation& operator=(const FixedDescriptorHeapReservation&) = delete;
    FixedDescriptorHeapReservation& operator=(FixedDescriptorHeapReservation&& other) noexcept;

    [[nodiscard]] bool isValid() const { return mHeap != nullptr && mReservedHeapBlocks.count > 0; }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(uint32_t index) const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(uint32_t index) const;

protected:
    FixedDescriptorHeapAllocator* mHeap = nullptr;
    FreeBlockTracker::Range mReservedHeapBlocks;
};

// Represents a contiguous block of allocated descriptors from a FixedDescriptorHeapAllocator. The allocation block held
// by this class can be further allocated and deallocated as needed. FixedDescriptorHeapSubAllocator will deallocate
// its allocation block from the FixedDescriptorHeapAllocator it orginally allocated from upon destruction.
class FixedDescriptorHeapSubAllocator final : public FixedDescriptorHeapReservation
{
public:
    FixedDescriptorHeapSubAllocator() = default;
    FixedDescriptorHeapSubAllocator(FixedDescriptorHeapAllocator& fixedHeap, FreeBlockTracker::Range range);
    FixedDescriptorHeapSubAllocator(const FixedDescriptorHeapSubAllocator&) = delete;
    FixedDescriptorHeapSubAllocator(FixedDescriptorHeapSubAllocator&&) noexcept = delete;
    ~FixedDescriptorHeapSubAllocator() final;

    FixedDescriptorHeapSubAllocator& operator=(const FixedDescriptorHeapSubAllocator&) = delete;
    FixedDescriptorHeapSubAllocator& operator=(FixedDescriptorHeapSubAllocator&&) noexcept = delete;

    tl::expected<uint32_t, FreeBlockTracker::Error> reserveUnsynchronized();
    tl::expected<uint32_t, FreeBlockTracker::Error> reserve();
    void releaseUnsynchronized(uint32_t index);
    void release(uint32_t index);
    void releaseAllUnsynchronized();
    void releaseAll();

private:
    FreeBlockTracker mSubAllocationTracker;
    std::mutex mMutex;
};

// Represents a contiguous block of allocated descriptors from a FixedDescriptorHeapAllocator. Descriptors can be
// allocated, but cannot be deallocated. Once the number of descriptors available have been allocated, no further
// allocations will succeed. FixedDescriptorHeapSubAllocator will deallocate its allocation block from the
// FixedDescriptorHeapAllocator it orginally allocated from upon destruction.
class FixedDescriptorHeapMonotonicSubAllocator final : public FixedDescriptorHeapReservation
{
public:
    FixedDescriptorHeapMonotonicSubAllocator() = default;
    FixedDescriptorHeapMonotonicSubAllocator(FixedDescriptorHeapAllocator& fixedHeap, FreeBlockTracker::Range range);
    FixedDescriptorHeapMonotonicSubAllocator(const FixedDescriptorHeapMonotonicSubAllocator&) = delete;
    FixedDescriptorHeapMonotonicSubAllocator(FixedDescriptorHeapMonotonicSubAllocator&& other) noexcept = delete;
    ~FixedDescriptorHeapMonotonicSubAllocator() final = default;

    FixedDescriptorHeapMonotonicSubAllocator& operator=(const FixedDescriptorHeapMonotonicSubAllocator&) = delete;
    FixedDescriptorHeapMonotonicSubAllocator&
    operator=(FixedDescriptorHeapMonotonicSubAllocator&& other) noexcept = delete;

    tl::expected<uint32_t, FreeBlockTracker::Error> reserve();

private:
    std::atomic_uint32_t mAllocationCount = 0;
};

// Represents a single descriptor allocation from a FixedDescriptorHeapAllocator
class FixedDescriptorHeapDescriptor
{
public:
    FixedDescriptorHeapDescriptor() = default;
    FixedDescriptorHeapDescriptor(FixedDescriptorHeapReservation&& reservation): mReservation(std::move(reservation)) {}

    FixedDescriptorHeapDescriptor(const FixedDescriptorHeapDescriptor&) = delete;
    FixedDescriptorHeapDescriptor(FixedDescriptorHeapDescriptor&&) noexcept = default;
    ~FixedDescriptorHeapDescriptor() = default;

    FixedDescriptorHeapDescriptor& operator=(const FixedDescriptorHeapDescriptor&) = delete;
    FixedDescriptorHeapDescriptor& operator=(FixedDescriptorHeapDescriptor&&) noexcept = default;

    [[nodiscard]] bool isValid() const { return mReservation.isValid(); }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle() const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle() const;

private:
    FixedDescriptorHeapReservation mReservation;
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
                                  const FixedDescriptorHeapReservation& allocation,
                                  uint32_t allocationIndex,
                                  const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createConstantBufferView(DeviceContext& context, const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    // SRV
    void createShaderResourceView(DeviceContext& context,
                                  const FixedDescriptorHeapReservation& allocation,
                                  uint32_t allocationIndex,
                                  ID3D12Resource* resource,
                                  const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createShaderResourceView(DeviceContext& context,
                             ID3D12Resource* resource,
                             const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    // UAV
    void createUnorderedAccessView(DeviceContext& context,
                                   const FixedDescriptorHeapReservation& allocation,
                                   uint32_t allocationIndex,
                                   ID3D12Resource* resource,
                                   ID3D12Resource* counterResource,
                                   const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createUnorderedAccessView(DeviceContext& context,
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
                       const FixedDescriptorHeapReservation& allocation,
                       uint32_t allocationIndex,
                       const D3D12_SAMPLER_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createSampler(DeviceContext& context, const D3D12_SAMPLER_DESC& desc);
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
                                const FixedDescriptorHeapReservation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* renderTargetResource,
                                const D3D12_RENDER_TARGET_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createRenderTargetView(DeviceContext& context,
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
                                const FixedDescriptorHeapReservation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* depthStencilResource,
                                const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createDepthStencilView(DeviceContext& context,
                           ID3D12Resource* depthStencilResource,
                           const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
};

} // namespace scrap::d3d12