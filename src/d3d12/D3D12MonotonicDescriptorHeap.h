// Classes:
//   scrap::d3d12::MonotonicDescriptorHeapAllocator
//   scrap::d3d12::MonotonicDescriptorHeapAllocation
//   scrap::d3d12::MonotonicDescriptorHeapDescriptor
//   scrap::d3d12::MonotonicDescriptorHeap_CBV_SRV_UAV
//   scrap::d3d12::MonotonicDescriptorHeap_Sampler
//   scrap::d3d12::MonotonicDescriptorHeap_RTV
//   scrap::d3d12::MonotonicDescriptorHeap_DSV
//
// WTF does monotonic mean:
//   https://en.wikipedia.org/wiki/Monotonic_function
//   https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource
//   Basically means a function that only ever increases or descreases, never both.
//
// This collection of classes manage allocations into d3d12 descriptor heaps. More specifically, these are monotonic
// descriptor heaps, meaning, once an descriptor is allocated, it will never be deallocated from the heap. Only when the
// entire heap is destroyed will the descriptors be invalidated. A monotonic descriptor heap is simpler to manage since
// descriptors will never leave the heap. This means the heap does not need to worry about managing descriptor lifetimes
// across multiple frames.
//
// A monotonic descriptor heap is useful for known render/depth-stencil targets that are consistent from frame to frame
// like the swap chain or a set of g-buffer targets. Samplers can also be a good use case for a monotonic descriptor
// heap.

#pragma once

#include "FreeBlockTracker.h"
#include "d3d12/D3D12Fwd.h"

#include <atomic>
#include <cstdint>

#include <tl/expected.hpp>
#include <wrl/client.h>

namespace scrap::d3d12
{
class MonotonicDescriptorHeapAllocation;

class MonotonicDescriptorHeapAllocator
{
public:
    MonotonicDescriptorHeapAllocator() = default;
    MonotonicDescriptorHeapAllocator(DeviceContext& context, const D3D12_DESCRIPTOR_HEAP_DESC& desc);
    MonotonicDescriptorHeapAllocator(const MonotonicDescriptorHeapAllocator&) = delete;
    MonotonicDescriptorHeapAllocator(MonotonicDescriptorHeapAllocator&&) noexcept = default;
    virtual ~MonotonicDescriptorHeapAllocator() = default;

    MonotonicDescriptorHeapAllocator& operator=(const MonotonicDescriptorHeapAllocator&) = delete;
    MonotonicDescriptorHeapAllocator& operator=(MonotonicDescriptorHeapAllocator&&) noexcept = default;

    [[nodiscard]] bool isValid() const { return mHeap; }

    [[nodiscard]] ID3D12DescriptorHeap* getDescriptorHeap() { return mHeap.Get(); }
    [[nodiscard]] uint32_t getDescriptorSize() const { return mDescriptorSize; }
    [[nodiscard]] uint32_t getDesctiptorCount() const { return mCapacity; }
    [[nodiscard]] size_t getByteSize() const { return static_cast<size_t>(mDescriptorSize) * mCapacity; }

    [[nodiscard]] tl::expected<MonotonicDescriptorHeapAllocation, FreeBlockTracker::Error>
    allocate(uint32_t descriptorCount);

protected:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mHeap;
    uint32_t mCapacity = 0u;
    std::atomic_uint32_t mAllocationCount = 0u;
    uint32_t mDescriptorSize = 0u;
};

class MonotonicDescriptorHeapAllocation
{
public:
    MonotonicDescriptorHeapAllocation() = default;
    MonotonicDescriptorHeapAllocation(MonotonicDescriptorHeapAllocator& permanentHeap, FreeBlockTracker::Range range);
    MonotonicDescriptorHeapAllocation(const MonotonicDescriptorHeapAllocation&) = delete;
    MonotonicDescriptorHeapAllocation(MonotonicDescriptorHeapAllocation&&) noexcept;
    ~MonotonicDescriptorHeapAllocation() = default;

    MonotonicDescriptorHeapAllocation& operator=(const MonotonicDescriptorHeapAllocation&) = delete;
    MonotonicDescriptorHeapAllocation& operator=(MonotonicDescriptorHeapAllocation&&) noexcept;

    [[nodiscard]] bool isValid() const { return mHeap != nullptr && mRange.count > 0; }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(uint32_t index) const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(uint32_t index) const;

private:
    MonotonicDescriptorHeapAllocator* mHeap = nullptr;
    FreeBlockTracker::Range mRange{};
};

class MonotonicDescriptorHeapDescriptor
{
public:
    MonotonicDescriptorHeapDescriptor() = default;
    MonotonicDescriptorHeapDescriptor(MonotonicDescriptorHeapAllocation&& allocation, uint32_t allocationIndex)
        : mAllocation(std::move(allocation))
        , mAllocationIndex(allocationIndex)
    {}

    MonotonicDescriptorHeapDescriptor(const MonotonicDescriptorHeapDescriptor&) = delete;
    MonotonicDescriptorHeapDescriptor(MonotonicDescriptorHeapDescriptor&&) noexcept = default;
    ~MonotonicDescriptorHeapDescriptor() = default;

    MonotonicDescriptorHeapDescriptor& operator=(const MonotonicDescriptorHeapDescriptor&) = delete;
    MonotonicDescriptorHeapDescriptor& operator=(MonotonicDescriptorHeapDescriptor&&) noexcept = default;

    [[nodiscard]] bool isValid() const { return mAllocation.isValid(); }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle() const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle() const;

private:
    MonotonicDescriptorHeapAllocation mAllocation;
    uint32_t mAllocationIndex = 0u;
};

class MonotonicDescriptorHeap_CBV_SRV_UAV final : public MonotonicDescriptorHeapAllocator
{
public:
    MonotonicDescriptorHeap_CBV_SRV_UAV() = default;
    MonotonicDescriptorHeap_CBV_SRV_UAV(DeviceContext& context, uint32_t numDescriptors);
    MonotonicDescriptorHeap_CBV_SRV_UAV(const MonotonicDescriptorHeap_CBV_SRV_UAV&) = delete;
    MonotonicDescriptorHeap_CBV_SRV_UAV(MonotonicDescriptorHeap_CBV_SRV_UAV&&) noexcept = default;
    ~MonotonicDescriptorHeap_CBV_SRV_UAV() final = default;

    MonotonicDescriptorHeap_CBV_SRV_UAV& operator=(const MonotonicDescriptorHeap_CBV_SRV_UAV&) = delete;
    MonotonicDescriptorHeap_CBV_SRV_UAV& operator=(MonotonicDescriptorHeap_CBV_SRV_UAV&&) noexcept = default;

    // CBV
    void createConstantBufferView(DeviceContext& context,
                                  const MonotonicDescriptorHeapAllocation& allocation,
                                  uint32_t allocationIndex,
                                  const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createConstantBufferView(DeviceContext& context, const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    // SRV
    void createShaderResourceView(DeviceContext& context,
                                  const MonotonicDescriptorHeapAllocation& allocation,
                                  uint32_t allocationIndex,
                                  ID3D12Resource* resource,
                                  const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createShaderResourceView(DeviceContext& context,
                             ID3D12Resource* resource,
                             const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    // UAV
    void createUnorderedAccessView(DeviceContext& context,
                                   const MonotonicDescriptorHeapAllocation& allocation,
                                   uint32_t allocationIndex,
                                   ID3D12Resource* resource,
                                   ID3D12Resource* counterResource,
                                   const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createUnorderedAccessView(DeviceContext& context,
                              ID3D12Resource* resource,
                              ID3D12Resource* counterResource,
                              const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);
};

class MonotonicDescriptorHeap_Sampler final : public MonotonicDescriptorHeapAllocator
{
public:
    MonotonicDescriptorHeap_Sampler() = default;
    MonotonicDescriptorHeap_Sampler(DeviceContext& context, uint32_t numDescriptors);
    MonotonicDescriptorHeap_Sampler(const MonotonicDescriptorHeap_Sampler&) = delete;
    MonotonicDescriptorHeap_Sampler(MonotonicDescriptorHeap_Sampler&&) noexcept = default;
    ~MonotonicDescriptorHeap_Sampler() final = default;

    MonotonicDescriptorHeap_Sampler& operator=(const MonotonicDescriptorHeap_Sampler&) = delete;
    MonotonicDescriptorHeap_Sampler& operator=(MonotonicDescriptorHeap_Sampler&&) noexcept = default;

    void createSampler(DeviceContext& context,
                       const MonotonicDescriptorHeapAllocation& allocation,
                       uint32_t allocationIndex,
                       const D3D12_SAMPLER_DESC& desc);

    [[nodiscard]] tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createSampler(DeviceContext& context, const D3D12_SAMPLER_DESC& desc);
};

class MonotonicDescriptorHeap_RTV final : public MonotonicDescriptorHeapAllocator
{
public:
    MonotonicDescriptorHeap_RTV() = default;
    MonotonicDescriptorHeap_RTV(DeviceContext& context, uint32_t numDescriptors);
    MonotonicDescriptorHeap_RTV(const MonotonicDescriptorHeap_RTV&) = delete;
    MonotonicDescriptorHeap_RTV(MonotonicDescriptorHeap_RTV&&) noexcept = default;
    ~MonotonicDescriptorHeap_RTV() final = default;

    MonotonicDescriptorHeap_RTV& operator=(const MonotonicDescriptorHeap_RTV&) = delete;
    MonotonicDescriptorHeap_RTV& operator=(MonotonicDescriptorHeap_RTV&&) noexcept = default;

    void createRenderTargetView(DeviceContext& context,
                                const MonotonicDescriptorHeapAllocation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* renderTargetResource,
                                const D3D12_RENDER_TARGET_VIEW_DESC* desc);

    [[nodiscard]] tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createRenderTargetView(DeviceContext& context,
                           ID3D12Resource* renderTargetResource,
                           const D3D12_RENDER_TARGET_VIEW_DESC* desc);
};

class MonotonicDescriptorHeap_DSV final : public MonotonicDescriptorHeapAllocator
{
public:
    MonotonicDescriptorHeap_DSV() = default;
    MonotonicDescriptorHeap_DSV(DeviceContext& context, uint32_t numDescriptors);
    MonotonicDescriptorHeap_DSV(const MonotonicDescriptorHeap_DSV&) = delete;
    MonotonicDescriptorHeap_DSV(MonotonicDescriptorHeap_DSV&&) noexcept = default;
    ~MonotonicDescriptorHeap_DSV() final = default;

    MonotonicDescriptorHeap_DSV& operator=(const MonotonicDescriptorHeap_DSV&) = delete;
    MonotonicDescriptorHeap_DSV& operator=(MonotonicDescriptorHeap_DSV&&) noexcept = default;

    void createDepthStencilView(DeviceContext& context,
                                const MonotonicDescriptorHeapAllocation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* depthStencilResource,
                                const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error>
    createDepthStencilView(DeviceContext& context,
                           ID3D12Resource* depthStencilResource,
                           const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
};

} // namespace scrap::d3d12