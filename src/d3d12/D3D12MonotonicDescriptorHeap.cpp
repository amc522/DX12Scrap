#include "D3D12MonotonicDescriptorHeap.h"

#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12Strings.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
MonotonicDescriptorHeapAllocator::MonotonicDescriptorHeapAllocator(D3D12Context& context,
                                                                   const D3D12_DESCRIPTOR_HEAP_DESC& desc)
{
    ComPtr<ID3D12DescriptorHeap> heap;
    HRESULT hr = context.getDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));

    if(FAILED(hr))
    {
        spdlog::error("Failed to create permanent descriptor heap. HRESULT = {}\n\tType: {}\n\tNumDescriptors: {}",
                      HRESULT_t(hr), desc.Type, desc.NumDescriptors);
        return;
    }

    mHeap = std::move(heap);
    mDescriptorSize = context.getDevice()->GetDescriptorHandleIncrementSize(desc.Type);
    mCapacity = desc.NumDescriptors;
}

tl::expected<MonotonicDescriptorHeapAllocation, FreeBlockTracker::Error> MonotonicDescriptorHeapAllocator::allocate(
    uint32_t descriptorCount)
{
    bool success = false;
    uint32_t newAllocationCount;
    uint32_t expectedCurrAllocationCount = mAllocationCount;
    do
    {
        newAllocationCount = expectedCurrAllocationCount + descriptorCount;
        if(newAllocationCount > mCapacity)
        {
            return tl::make_unexpected(FreeBlockTracker::Error::InsufficientCapacity);
        }

        success = mAllocationCount.compare_exchange_weak(expectedCurrAllocationCount, newAllocationCount);
    } while(!success);

    uint32_t startIndex = expectedCurrAllocationCount;

    return MonotonicDescriptorHeapAllocation(*this, FreeBlockTracker::Range{startIndex, descriptorCount});
}

MonotonicDescriptorHeapAllocation::MonotonicDescriptorHeapAllocation(MonotonicDescriptorHeapAllocator& heapAllocator,
                                                                     FreeBlockTracker::Range range)
    : mHeap(&heapAllocator), mRange(range)
{
}

MonotonicDescriptorHeapAllocation::MonotonicDescriptorHeapAllocation(MonotonicDescriptorHeapAllocation&& other) noexcept
    : mHeap(other.mHeap), mRange(other.mRange)
{
    other.mHeap = nullptr;
}

MonotonicDescriptorHeapAllocation& MonotonicDescriptorHeapAllocation::operator=(
    MonotonicDescriptorHeapAllocation&& other) noexcept
{
    mHeap = other.mHeap;
    mRange = other.mRange;
    other.mHeap = nullptr;

    return *this;
}

D3D12_CPU_DESCRIPTOR_HANDLE MonotonicDescriptorHeapAllocation::getCpuHandle(uint32_t index) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE{mHeap->getDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), (INT)index,
                                         mHeap->getDescriptorSize()};
}

D3D12_GPU_DESCRIPTOR_HANDLE MonotonicDescriptorHeapAllocation::getGpuHandle(uint32_t index) const
{
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(mHeap->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(), (INT)index,
                                         mHeap->getDescriptorSize());
}

MonotonicDescriptorHeap_CBV_SRV_UAV::MonotonicDescriptorHeap_CBV_SRV_UAV(D3D12Context& context, uint32_t numDescriptors)
    : MonotonicDescriptorHeapAllocator(context,
                                       D3D12_DESCRIPTOR_HEAP_DESC{D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                                  numDescriptors,
                                                                  D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0})
{
}

void MonotonicDescriptorHeap_CBV_SRV_UAV::createConstantBufferView(D3D12Context& context,
                                                                   const MonotonicDescriptorHeapAllocation& allocation,
                                                                   uint32_t allocationIndex,
                                                                   const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
{
    context.getDevice()->CreateConstantBufferView(&desc, allocation.getCpuHandle(allocationIndex));
}

tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error> MonotonicDescriptorHeap_CBV_SRV_UAV::
    createConstantBufferView(D3D12Context& context, const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
{
    auto allocation = allocate(1);
    if(!allocation) { return tl::make_unexpected(allocation.error()); }

    MonotonicDescriptorHeapDescriptor descriptor{std::move(allocation.value()), 0};
    context.getDevice()->CreateConstantBufferView(&desc, descriptor.getCpuHandle());
    return descriptor;
}

void MonotonicDescriptorHeap_CBV_SRV_UAV::createShaderResourceView(D3D12Context& context,
                                                                   const MonotonicDescriptorHeapAllocation& allocation,
                                                                   uint32_t allocationIndex,
                                                                   ID3D12Resource* resource,
                                                                   const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
{
    context.getDevice()->CreateShaderResourceView(resource, &desc, allocation.getCpuHandle(allocationIndex));
}

tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error> MonotonicDescriptorHeap_CBV_SRV_UAV::
    createShaderResourceView(D3D12Context& context,
                             ID3D12Resource* resource,
                             const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
{
    auto allocation = allocate(1);
    if(!allocation) { return tl::make_unexpected(allocation.error()); }

    MonotonicDescriptorHeapDescriptor descriptor{std::move(allocation.value()), 0};
    context.getDevice()->CreateShaderResourceView(resource, &desc, descriptor.getCpuHandle());
    return descriptor;
}

void MonotonicDescriptorHeap_CBV_SRV_UAV::createUnorderedAccessView(D3D12Context& context,
                                                                    const MonotonicDescriptorHeapAllocation& allocation,
                                                                    uint32_t allocationIndex,
                                                                    ID3D12Resource* resource,
                                                                    ID3D12Resource* counterResource,
                                                                    const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
{
    context.getDevice()->CreateUnorderedAccessView(resource, counterResource, &desc,
                                                   allocation.getCpuHandle(allocationIndex));
}

tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error> MonotonicDescriptorHeap_CBV_SRV_UAV::
    createUnorderedAccessView(D3D12Context& context,
                              ID3D12Resource* resource,
                              ID3D12Resource* counterResource,
                              const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
{
    auto allocation = allocate(1);
    if(!allocation) { return tl::make_unexpected(allocation.error()); }

    MonotonicDescriptorHeapDescriptor descriptor{std::move(allocation.value()), 0};
    context.getDevice()->CreateUnorderedAccessView(resource, counterResource, &desc, descriptor.getCpuHandle());
    return descriptor;
}

MonotonicDescriptorHeap_Sampler::MonotonicDescriptorHeap_Sampler(D3D12Context& context, uint32_t numDescriptors)
    : MonotonicDescriptorHeapAllocator(context,
                                       D3D12_DESCRIPTOR_HEAP_DESC{D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, numDescriptors,
                                                                  D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0})
{
}

void MonotonicDescriptorHeap_Sampler::createSampler(D3D12Context& context,
                                                    const MonotonicDescriptorHeapAllocation& allocation,
                                                    uint32_t allocationIndex,
                                                    const D3D12_SAMPLER_DESC& desc)
{
    context.getDevice()->CreateSampler(&desc, allocation.getCpuHandle(allocationIndex));
}

tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error> MonotonicDescriptorHeap_Sampler::createSampler(
    D3D12Context& context,
    const D3D12_SAMPLER_DESC& desc)
{
    auto allocation = allocate(1);
    if(!allocation) { return tl::make_unexpected(allocation.error()); }

    MonotonicDescriptorHeapDescriptor descriptor{std::move(allocation.value()), 0};
    context.getDevice()->CreateSampler(&desc, descriptor.getCpuHandle());
    return descriptor;
}

MonotonicDescriptorHeap_RTV::MonotonicDescriptorHeap_RTV(D3D12Context& context, uint32_t numDescriptors)
    : MonotonicDescriptorHeapAllocator(context,
                                       D3D12_DESCRIPTOR_HEAP_DESC{D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors,
                                                                  D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0})
{
}

void MonotonicDescriptorHeap_RTV::createRenderTargetView(D3D12Context& context,
                                                         const MonotonicDescriptorHeapAllocation& allocation,
                                                         uint32_t allocationIndex,
                                                         ID3D12Resource* renderTarget,
                                                         const D3D12_RENDER_TARGET_VIEW_DESC& desc)
{
    context.getDevice()->CreateRenderTargetView(renderTarget, &desc, allocation.getCpuHandle(allocationIndex));
}

tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error> MonotonicDescriptorHeap_RTV::createRenderTargetView(
    D3D12Context& context,
    ID3D12Resource* renderTarget,
    const D3D12_RENDER_TARGET_VIEW_DESC& desc)
{
    auto allocation = allocate(1);
    if(!allocation) { return tl::make_unexpected(allocation.error()); }

    MonotonicDescriptorHeapDescriptor descriptor{std::move(allocation.value()), 0};
    context.getDevice()->CreateRenderTargetView(renderTarget, &desc, descriptor.getCpuHandle());
    return descriptor;
}
MonotonicDescriptorHeap_DSV::MonotonicDescriptorHeap_DSV(D3D12Context& context, uint32_t numDescriptors)
    : MonotonicDescriptorHeapAllocator(context,
                                       D3D12_DESCRIPTOR_HEAP_DESC{D3D12_DESCRIPTOR_HEAP_TYPE_DSV, numDescriptors,
                                                                  D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0})
{
}

void MonotonicDescriptorHeap_DSV::createDepthStencilView(D3D12Context& context,
                                                         const MonotonicDescriptorHeapAllocation& allocation,
                                                         uint32_t allocationIndex,
                                                         ID3D12Resource* depthStencilResource,
                                                         const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
{
    context.getDevice()->CreateDepthStencilView(depthStencilResource, &desc, allocation.getCpuHandle(allocationIndex));
}

tl::expected<MonotonicDescriptorHeapDescriptor, FreeBlockTracker::Error> MonotonicDescriptorHeap_DSV::createDepthStencilView(
    D3D12Context& context,
    ID3D12Resource* depthStencilResource,
    const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
{
    auto allocation = allocate(1);
    if(!allocation) { return tl::make_unexpected(allocation.error()); }

    MonotonicDescriptorHeapDescriptor descriptor{std::move(allocation.value()), 0};
    context.getDevice()->CreateDepthStencilView(depthStencilResource, &desc, descriptor.getCpuHandle());
    return descriptor;
}

D3D12_CPU_DESCRIPTOR_HANDLE MonotonicDescriptorHeapDescriptor::getCpuHandle() const
{
    return mAllocation.getCpuHandle(mAllocationIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE MonotonicDescriptorHeapDescriptor::getGpuHandle() const
{
    return mAllocation.getGpuHandle(mAllocationIndex);
}
} // namespace scrap::d3d12