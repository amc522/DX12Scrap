// Classes:
//   scrap::d3d12::PermanentDescriptorHeapAllocator
//   scrap::d3d12::PermanentDescriptorHeapAllocation
//   scrap::d3d12::PermanentDescriptorHeapDescriptor
//   scrap::d3d12::PermanentDescriptorHeap_CBV_SRV_UAV
//   scrap::d3d12::PermanentDescriptorHeap_Sampler
//   scrap::d3d12::PermanentDescriptorHeap_RTV
//   scrap::d3d12::PermanentDescriptorHeap_DSV
//
// This collection of classes manage allocations into d3d12 descriptor heaps. More specifically, these are permanent
// descriptor heaps, meaning, once an descriptor is allocated, it will never be deallocated from the heap. Only when the
// entire heap is destroyed will the descriptors be invalidated. A permanent descriptor heap is simpler to manage since
// descriptors will never leave the heap. This means the heap does not need to worry about managing descriptor lifetimes
// across multiple frames.
//
// A permanent descriptor heap is useful for known render/depth-stencil targets that are consistent from frame to frame
// like the swap chain or a set of g-buffer targets. Samplers can also be a good use case for a permanent descriptor.

#pragma once

#include "FreeBlockTracker.h"

#include <atomic>
#include <cstdint>

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

namespace scrap
{
class D3D12Context;
}

namespace scrap::d3d12
{
class PermanentDescriptorHeapAllocation;

class PermanentDescriptorHeapAllocator
{
public:
    PermanentDescriptorHeapAllocator() = default;
    PermanentDescriptorHeapAllocator(D3D12Context& context, const D3D12_DESCRIPTOR_HEAP_DESC& desc);
    PermanentDescriptorHeapAllocator(const PermanentDescriptorHeapAllocator&) = delete;
    PermanentDescriptorHeapAllocator(PermanentDescriptorHeapAllocator&&) noexcept = default;
    virtual ~PermanentDescriptorHeapAllocator() = default;

    PermanentDescriptorHeapAllocator& operator=(const PermanentDescriptorHeapAllocator&) = delete;
    PermanentDescriptorHeapAllocator& operator=(PermanentDescriptorHeapAllocator&&) noexcept = default;

    [[nodiscard]] bool isValid() const { return mHeap; }

    [[nodiscard]] ID3D12DescriptorHeap* getDescriptorHeap() { return mHeap.Get(); }
    [[nodiscard]] uint32_t getDescriptorSize() const { return mDescriptorSize; }
    [[nodiscard]] uint32_t getDesctiptorCount() const { return mCapacity; }
    [[nodiscard]] size_t getByteSize() const { return static_cast<size_t>(mDescriptorSize) * mCapacity; }

    [[nodiscard]] tl::expected<PermanentDescriptorHeapAllocation, FreeBlockTracker::Error> allocate(
        uint32_t descriptorCount);

protected:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mHeap;
    uint32_t mCapacity = 0u;
    std::atomic_uint32_t mAllocationCount = 0u;
    uint32_t mDescriptorSize = 0u;
};

class PermanentDescriptorHeapAllocation
{
public:
    PermanentDescriptorHeapAllocation() = default;
    PermanentDescriptorHeapAllocation(PermanentDescriptorHeapAllocator& permanentHeap, FreeBlockTracker::Range range);
    PermanentDescriptorHeapAllocation(const PermanentDescriptorHeapAllocation&) = delete;
    PermanentDescriptorHeapAllocation(PermanentDescriptorHeapAllocation&&) noexcept;
    ~PermanentDescriptorHeapAllocation() = default;

    PermanentDescriptorHeapAllocation& operator=(const PermanentDescriptorHeapAllocation&) = delete;
    PermanentDescriptorHeapAllocation& operator=(PermanentDescriptorHeapAllocation&&) noexcept;

    [[nodiscard]] bool isValid() const { return mHeap != nullptr && mRange.count > 0; }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(uint32_t index) const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(uint32_t index) const;

private:
    PermanentDescriptorHeapAllocator* mHeap = nullptr;
    FreeBlockTracker::Range mRange{};
};

class PermanentDescriptorHeapDescriptor
{
public:
    PermanentDescriptorHeapDescriptor() = default;
    PermanentDescriptorHeapDescriptor(PermanentDescriptorHeapAllocation&& allocation, uint32_t allocationIndex)
        : mAllocation(std::move(allocation)), mAllocationIndex(allocationIndex)
    {
    }

    PermanentDescriptorHeapDescriptor(const PermanentDescriptorHeapDescriptor&) = delete;
    PermanentDescriptorHeapDescriptor(PermanentDescriptorHeapDescriptor&&) noexcept = default;
    ~PermanentDescriptorHeapDescriptor() = default;

    PermanentDescriptorHeapDescriptor& operator=(const PermanentDescriptorHeapDescriptor&) = delete;
    PermanentDescriptorHeapDescriptor& operator=(PermanentDescriptorHeapDescriptor&&) noexcept = default;

    [[nodiscard]] bool isValid() const { return mAllocation.isValid(); }

    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle() const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle() const;

private:
    PermanentDescriptorHeapAllocation mAllocation;
    uint32_t mAllocationIndex = 0u;
};

class PermanentDescriptorHeap_CBV_SRV_UAV final : public PermanentDescriptorHeapAllocator
{
public:
    PermanentDescriptorHeap_CBV_SRV_UAV() = default;
    PermanentDescriptorHeap_CBV_SRV_UAV(D3D12Context& context, uint32_t numDescriptors);
    PermanentDescriptorHeap_CBV_SRV_UAV(const PermanentDescriptorHeap_CBV_SRV_UAV&) = delete;
    PermanentDescriptorHeap_CBV_SRV_UAV(PermanentDescriptorHeap_CBV_SRV_UAV&&) noexcept = default;
    ~PermanentDescriptorHeap_CBV_SRV_UAV() final = default;

    PermanentDescriptorHeap_CBV_SRV_UAV& operator=(const PermanentDescriptorHeap_CBV_SRV_UAV&) = delete;
    PermanentDescriptorHeap_CBV_SRV_UAV& operator=(PermanentDescriptorHeap_CBV_SRV_UAV&&) noexcept = default;

    // CBV
    void createConstantBufferView(D3D12Context& context,
                                  const PermanentDescriptorHeapAllocation& allocation,
                                  uint32_t allocationIndex,
                                  const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<PermanentDescriptorHeapDescriptor, FreeBlockTracker::Error> createConstantBufferView(
        D3D12Context& context,
        const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

    // SRV
    void createShaderResourceView(D3D12Context& context,
                                  const PermanentDescriptorHeapAllocation& allocation,
                                  uint32_t allocationIndex,
                                  ID3D12Resource* resource,
                                  const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<PermanentDescriptorHeapDescriptor, FreeBlockTracker::Error> createShaderResourceView(
        D3D12Context& context,
        ID3D12Resource* resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    // UAV
    void createUnorderedAccessView(D3D12Context& context,
                                   const PermanentDescriptorHeapAllocation& allocation,
                                   uint32_t allocationIndex,
                                   ID3D12Resource* resource,
                                   ID3D12Resource* counterResource,
                                   const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<PermanentDescriptorHeapDescriptor, FreeBlockTracker::Error> createUnorderedAccessView(
        D3D12Context& context,
        ID3D12Resource* resource,
        ID3D12Resource* counterResource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);
};

class PermanentDescriptorHeap_Sampler final : public PermanentDescriptorHeapAllocator
{
public:
    PermanentDescriptorHeap_Sampler() = default;
    PermanentDescriptorHeap_Sampler(D3D12Context& context, uint32_t numDescriptors);
    PermanentDescriptorHeap_Sampler(const PermanentDescriptorHeap_Sampler&) = delete;
    PermanentDescriptorHeap_Sampler(PermanentDescriptorHeap_Sampler&&) noexcept = default;
    ~PermanentDescriptorHeap_Sampler() final = default;

    PermanentDescriptorHeap_Sampler& operator=(const PermanentDescriptorHeap_Sampler&) = delete;
    PermanentDescriptorHeap_Sampler& operator=(PermanentDescriptorHeap_Sampler&&) noexcept = default;

    void createSampler(D3D12Context& context,
                       const PermanentDescriptorHeapAllocation& allocation,
                       uint32_t allocationIndex,
                       const D3D12_SAMPLER_DESC& desc);

    [[nodiscard]] tl::expected<PermanentDescriptorHeapDescriptor, FreeBlockTracker::Error> createSampler(
        D3D12Context& context,
        const D3D12_SAMPLER_DESC& desc);
};

class PermanentDescriptorHeap_RTV final : public PermanentDescriptorHeapAllocator
{
public:
    PermanentDescriptorHeap_RTV() = default;
    PermanentDescriptorHeap_RTV(D3D12Context& context, uint32_t numDescriptors);
    PermanentDescriptorHeap_RTV(const PermanentDescriptorHeap_RTV&) = delete;
    PermanentDescriptorHeap_RTV(PermanentDescriptorHeap_RTV&&) noexcept = default;
    ~PermanentDescriptorHeap_RTV() final = default;

    PermanentDescriptorHeap_RTV& operator=(const PermanentDescriptorHeap_RTV&) = delete;
    PermanentDescriptorHeap_RTV& operator=(PermanentDescriptorHeap_RTV&&) noexcept = default;

    void createRenderTargetView(D3D12Context& context,
                                const PermanentDescriptorHeapAllocation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* renderTargetResource,
                                const D3D12_RENDER_TARGET_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<PermanentDescriptorHeapDescriptor, FreeBlockTracker::Error> createRenderTargetView(
        D3D12Context& context,
        ID3D12Resource* renderTargetResource,
        const D3D12_RENDER_TARGET_VIEW_DESC& desc);
};

class PermanentDescriptorHeap_DSV final : public PermanentDescriptorHeapAllocator
{
public:
    PermanentDescriptorHeap_DSV() = default;
    PermanentDescriptorHeap_DSV(D3D12Context& context, uint32_t numDescriptors);
    PermanentDescriptorHeap_DSV(const PermanentDescriptorHeap_DSV&) = delete;
    PermanentDescriptorHeap_DSV(PermanentDescriptorHeap_DSV&&) noexcept = default;
    ~PermanentDescriptorHeap_DSV() final = default;

    PermanentDescriptorHeap_DSV& operator=(const PermanentDescriptorHeap_DSV&) = delete;
    PermanentDescriptorHeap_DSV& operator=(PermanentDescriptorHeap_DSV&&) noexcept = default;

    void createDepthStencilView(D3D12Context& context,
                                const PermanentDescriptorHeapAllocation& allocation,
                                uint32_t allocationIndex,
                                ID3D12Resource* depthStencilResource,
                                const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);

    [[nodiscard]] tl::expected<PermanentDescriptorHeapDescriptor, FreeBlockTracker::Error> createDepthStencilView(
        D3D12Context& context,
        ID3D12Resource* depthStencilResource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
};

} // namespace scrap::d3d12