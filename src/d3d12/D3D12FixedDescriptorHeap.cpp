#include "D3D12FixedDescriptorHeap.h"

#include "D3D12Strings.h"
#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <spdlog/spdlog.h>

namespace scrap::d3d12
{
FixedDescriptorHeapAllocator::FixedDescriptorHeapAllocator(DeviceContext& context,
                                                           D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                                                           uint32_t descriptorCount)
{
    D3D12_DESCRIPTOR_HEAP_DESC cpuDesc = {};
    cpuDesc.Type = heapType;
    cpuDesc.NumDescriptors = descriptorCount;
    cpuDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createdescriptorheap
    HRESULT hr = context.getDevice()->CreateDescriptorHeap(&cpuDesc, IID_PPV_ARGS(&mCpuHeap));
    if(FAILED(hr))
    {
        spdlog::error("Failed to create fixed descriptor heap (cpu). HRESULT = {}\n\tType: {}\n\tNumDescriptors: {}",
                      HRESULT_t(hr), heapType, descriptorCount);
        return;
    }

    D3D12_DESCRIPTOR_HEAP_DESC gpuDesc = cpuDesc;
    gpuDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    hr = context.getDevice()->CreateDescriptorHeap(&gpuDesc, IID_PPV_ARGS(&mGpuHeap));
    if(FAILED(hr))
    {
        spdlog::error("Failed to create fixed descriptor heap (gpu). HRESULT = {}\n\tType: {}\n\tNumDescriptors: {}",
                      HRESULT_t(hr), heapType, descriptorCount);
        return;
    }

    mDescriptorSize = context.getDevice()->GetDescriptorHandleIncrementSize(heapType);

    mFreeBlockTracker = FreeBlockTracker(descriptorCount);
}

FixedDescriptorHeapAllocator::~FixedDescriptorHeapAllocator()
{
    assert(mFreeBlockTracker.getReservedBlockCount() == 0);
}

tl::expected<std::shared_ptr<FixedDescriptorHeapSubAllocator>, FreeBlockTracker::Error>
FixedDescriptorHeapAllocator::allocateSubHeap(uint32_t descriptorCount)
{
    std::lock_guard lockGuard{mMutex};

    auto reservation = mFreeBlockTracker.unsafeReserve(descriptorCount);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    FreeBlockTracker::Range range{reservation.value(), descriptorCount};
    mCpuRangesToCopy.push_back(range);

    return std::make_shared<FixedDescriptorHeapSubAllocator>(*this, range);
}

tl::expected<std::shared_ptr<FixedDescriptorHeapMonotonicSubAllocator>, FreeBlockTracker::Error>
FixedDescriptorHeapAllocator::allocateMonotonicSubHeap(uint32_t descriptorCount)
{
    std::lock_guard lockGuard{mMutex};

    auto reservation = mFreeBlockTracker.unsafeReserve(descriptorCount);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    FreeBlockTracker::Range range{reservation.value(), descriptorCount};
    mCpuRangesToCopy.push_back(range);

    return std::make_shared<FixedDescriptorHeapMonotonicSubAllocator>(*this, range);
}

tl::expected<FixedDescriptorHeapReservation, FreeBlockTracker::Error>
FixedDescriptorHeapAllocator::reserve(uint32_t descriptorCount)
{
    std::lock_guard lockGuard{ mMutex };

    auto reservation = mFreeBlockTracker.unsafeReserve(descriptorCount);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    FreeBlockTracker::Range range{ reservation.value(), descriptorCount };
    mCpuRangesToCopy.push_back(range);

    return FixedDescriptorHeapReservation(*this, range);
}

void FixedDescriptorHeapAllocator::deallocate(FreeBlockTracker::Range range)
{
    std::lock_guard lockGuard{mMutex};

    // check to see if the range is still queued for copy.
    for(auto itr = mCpuRangesToCopy.begin(); itr != mCpuRangesToCopy.end(); ++itr)
    {
        if(!itr->contains(range)) { continue; }

        if(*itr == range)
        {
            // exact range, just erase
            mCpuRangesToCopy.erase(itr);
            break;
        }
        else if(itr->start == range.start)
        {
            // trim the front of the copy range
            itr->start += range.count;
        }
        else if(itr->exclusiveEnd() == range.exclusiveEnd())
        {
            // trim end end of the copy range
            itr->count -= range.count;
        }
        else
        {
            // the range is in the middle and needs to be split
            FreeBlockTracker::Range newRange{range.exclusiveEnd(), itr->exclusiveEnd() - range.inclusiveEnd()};
            itr->count -= range.count - newRange.count;
            mCpuRangesToCopy.insert(itr + 1, newRange);
        }

        break;
    }

    mFreeBlockTracker.unsafeRelease(range);
}

void FixedDescriptorHeapAllocator::uploadPendingDescriptors(DeviceContext& context)
{
    std::lock_guard lockGuard{mMutex};

    if(mCpuRangesToCopy.empty()) { return; }

    auto copyDescriptors = [&](FreeBlockTracker::Range range) {
        D3D12_CPU_DESCRIPTOR_HANDLE dest = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mGpuHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<UINT>(range.start), mDescriptorSize);
        D3D12_CPU_DESCRIPTOR_HANDLE source = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mCpuHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<UINT>(range.start), mDescriptorSize);

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-copydescriptorssimple
        context.getDevice()->CopyDescriptorsSimple(static_cast<UINT>(range.count), dest, source,
                                                   mCpuHeap->GetDesc().Type);
    };

    if(mCpuRangesToCopy.size() == 1u)
    {
        copyDescriptors(mCpuRangesToCopy.front());
        mCpuRangesToCopy.clear();
        return;
    }

    // sort the ranges so they can ben easily merged
    std::sort(mCpuRangesToCopy.begin(), mCpuRangesToCopy.end(),
              [](const FreeBlockTracker::Range& left, const FreeBlockTracker::Range& right) {
                  return left.start < right.start;
              });

    // Merge and copy consecutive ranges
    auto itr = mCpuRangesToCopy.cbegin();
    FreeBlockTracker::Range mergedRange = *itr;
    ++itr;
    while(itr != mCpuRangesToCopy.end())
    {
        if(!mergedRange.merge(*itr))
        {
            copyDescriptors(mergedRange);
            mergedRange = *itr;
        }

        ++itr;
    }

    copyDescriptors(mergedRange);
    mCpuRangesToCopy.clear();
}

FixedDescriptorHeapReservation::FixedDescriptorHeapReservation(FixedDescriptorHeapAllocator& fixedHeap,
                                                               FreeBlockTracker::Range range)
    : mHeap(&fixedHeap)
    , mReservedHeapBlocks(range)
{}

FixedDescriptorHeapReservation::FixedDescriptorHeapReservation(FixedDescriptorHeapReservation&& other) noexcept
    : mHeap(other.mHeap)
    , mReservedHeapBlocks(std::move(other.mReservedHeapBlocks))
{
    other.mHeap = nullptr;
}

FixedDescriptorHeapReservation::~FixedDescriptorHeapReservation()
{
    if(mHeap != nullptr) { mHeap->deallocate(mReservedHeapBlocks); }
}

D3D12_CPU_DESCRIPTOR_HANDLE FixedDescriptorHeapReservation::getCpuHandle(uint32_t index) const
{
    assert(index < mReservedHeapBlocks.count);
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mHeap->getCpuDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
                                         static_cast<INT>(mReservedHeapBlocks.start + index),
                                         mHeap->getDescriptorSize());
}

D3D12_GPU_DESCRIPTOR_HANDLE FixedDescriptorHeapReservation::getGpuHandle(uint32_t index) const
{
    assert(index < mReservedHeapBlocks.count);
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(mHeap->getGpuDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
                                         static_cast<INT>(mReservedHeapBlocks.start + index),
                                         mHeap->getDescriptorSize());
}

FixedDescriptorHeapReservation&
FixedDescriptorHeapReservation::operator=(FixedDescriptorHeapReservation&& other) noexcept
{
    if(mHeap != nullptr) { mHeap->deallocate(mReservedHeapBlocks); }

    mHeap = other.mHeap;
    mReservedHeapBlocks = std::move(other.mReservedHeapBlocks);
    other.mHeap = nullptr;

    return *this;
}

FixedDescriptorHeapSubAllocator::FixedDescriptorHeapSubAllocator(FixedDescriptorHeapAllocator& fixedHeap,
                                                                 FreeBlockTracker::Range range)
    : FixedDescriptorHeapReservation(fixedHeap, range)
{}

FixedDescriptorHeapSubAllocator::~FixedDescriptorHeapSubAllocator()
{
    assert(mSubAllocationTracker.getReservedBlockCount() == 0);
}

tl::expected<uint32_t, FreeBlockTracker::Error> FixedDescriptorHeapSubAllocator::reserveUnsynchronized()
{
    auto reservation = mSubAllocationTracker.unsafeReserve(1);

    if(!reservation) { return tl::make_unexpected(reservation.error()); }
    return static_cast<uint32_t>(reservation.value());
}

tl::expected<uint32_t, FreeBlockTracker::Error> FixedDescriptorHeapSubAllocator::reserve()
{
    std::lock_guard lockGuard{mMutex};

    return reserveUnsynchronized();
}

void FixedDescriptorHeapSubAllocator::releaseUnsynchronized(uint32_t index)
{
    mSubAllocationTracker.unsafeRelease(index, 1);
}

void FixedDescriptorHeapSubAllocator::release(uint32_t index)
{
    std::lock_guard lockGuard{mMutex};
    releaseUnsynchronized(index);
}

void FixedDescriptorHeapSubAllocator::releaseAllUnsynchronized()
{
    mSubAllocationTracker.unsafeRelease(0, mSubAllocationTracker.getCapacity());
}

void FixedDescriptorHeapSubAllocator::releaseAll()
{
    std::lock_guard lockGuard(mMutex);
    releaseAllUnsynchronized();
}

FixedDescriptorHeapMonotonicSubAllocator::FixedDescriptorHeapMonotonicSubAllocator(
    FixedDescriptorHeapAllocator& fixedHeap,
    FreeBlockTracker::Range range)
    : FixedDescriptorHeapReservation(fixedHeap, range)
{}

tl::expected<uint32_t, FreeBlockTracker::Error> FixedDescriptorHeapMonotonicSubAllocator::reserve()
{
    bool success = false;
    uint32_t newAllocationCount;
    uint32_t expectedCurrAllocationCount = mAllocationCount;
    do
    {
        newAllocationCount = expectedCurrAllocationCount + 1;
        if(newAllocationCount > mReservedHeapBlocks.count)
        {
            return tl::make_unexpected(FreeBlockTracker::Error::InsufficientCapacity);
        }

        success = mAllocationCount.compare_exchange_weak(expectedCurrAllocationCount, newAllocationCount);
    } while(!success);

    uint32_t startIndex = expectedCurrAllocationCount;

    return newAllocationCount;
}

D3D12_CPU_DESCRIPTOR_HANDLE FixedDescriptorHeapDescriptor::getCpuHandle() const
{
    return mReservation.getCpuHandle(0);
}
D3D12_GPU_DESCRIPTOR_HANDLE FixedDescriptorHeapDescriptor::getGpuHandle() const
{
    return mReservation.getGpuHandle(0);
}

FixedDescriptorHeap_CBV_SRV_UAV::FixedDescriptorHeap_CBV_SRV_UAV(DeviceContext& context, uint32_t descriptorCount)
    : FixedDescriptorHeapAllocator(context, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorCount)
{}

void FixedDescriptorHeap_CBV_SRV_UAV::createConstantBufferView(DeviceContext& context,
                                                               const FixedDescriptorHeapReservation& reservation,
                                                               uint32_t allocationIndex,
                                                               const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createconstantbufferview
    context.getDevice()->CreateConstantBufferView(&desc, reservation.getCpuHandle(allocationIndex));
}

tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
FixedDescriptorHeap_CBV_SRV_UAV::createConstantBufferView(DeviceContext& context,
                                                          const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
{
    auto reservation = reserve(1);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    createConstantBufferView(context, reservation.value(), 0, desc);

    return FixedDescriptorHeapDescriptor{std::move(reservation.value())};
}

void FixedDescriptorHeap_CBV_SRV_UAV::createShaderResourceView(DeviceContext& context,
                                                               const FixedDescriptorHeapReservation& reservation,
                                                               uint32_t allocationIndex,
                                                               ID3D12Resource* resource,
                                                               const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createshaderresourceview
    context.getDevice()->CreateShaderResourceView(resource, &desc, reservation.getCpuHandle(allocationIndex));
}

tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
FixedDescriptorHeap_CBV_SRV_UAV::createShaderResourceView(DeviceContext& context,
                                                          ID3D12Resource* resource,
                                                          const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
{
    auto reservation = reserve(1);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    createShaderResourceView(context, reservation.value(), 0, resource, desc);

    return FixedDescriptorHeapDescriptor{std::move(reservation.value())};
}

void FixedDescriptorHeap_CBV_SRV_UAV::createUnorderedAccessView(DeviceContext& context,
                                                                const FixedDescriptorHeapReservation& reservation,
                                                                uint32_t allocationIndex,
                                                                ID3D12Resource* resource,
                                                                ID3D12Resource* counterResource,
                                                                const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createunorderedaccessview
    context.getDevice()->CreateUnorderedAccessView(resource, counterResource, &desc,
                                                   reservation.getCpuHandle(allocationIndex));
}

tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
FixedDescriptorHeap_CBV_SRV_UAV::createUnorderedAccessView(DeviceContext& context,
                                                           ID3D12Resource* resource,
                                                           ID3D12Resource* counterResource,
                                                           const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
{
    auto reservation = reserve(1);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    createUnorderedAccessView(context, reservation.value(), 0, resource, counterResource, desc);

    return FixedDescriptorHeapDescriptor{std::move(reservation.value())};
}

FixedDescriptorHeap_Sampler::FixedDescriptorHeap_Sampler(DeviceContext& context, uint32_t descriptorCount)
    : FixedDescriptorHeapAllocator(context, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, descriptorCount)
{}

void FixedDescriptorHeap_Sampler::createSampler(DeviceContext& context,
                                                const FixedDescriptorHeapReservation& reservation,
                                                uint32_t allocationIndex,
                                                const D3D12_SAMPLER_DESC& desc)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createsampler
    context.getDevice()->CreateSampler(&desc, reservation.getCpuHandle(allocationIndex));
}

tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
FixedDescriptorHeap_Sampler::createSampler(DeviceContext& context, const D3D12_SAMPLER_DESC& desc)
{
    auto reservation = reserve(1);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    createSampler(context, reservation.value(), 0, desc);

    return FixedDescriptorHeapDescriptor{std::move(reservation.value())};
}

FixedDescriptorHeap_RTV::FixedDescriptorHeap_RTV(DeviceContext& context, uint32_t descriptorCount)
    : FixedDescriptorHeapAllocator(context, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, descriptorCount)
{}

void FixedDescriptorHeap_RTV::createRenderTargetView(DeviceContext& context,
                                                     const FixedDescriptorHeapReservation& reservation,
                                                     uint32_t allocationIndex,
                                                     ID3D12Resource* renderTargetResource,
                                                     const D3D12_RENDER_TARGET_VIEW_DESC& desc)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createrendertargetview
    context.getDevice()->CreateRenderTargetView(renderTargetResource, &desc, reservation.getCpuHandle(allocationIndex));
}

tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
FixedDescriptorHeap_RTV::createRenderTargetView(DeviceContext& context,
                                                ID3D12Resource* renderTargetResource,
                                                const D3D12_RENDER_TARGET_VIEW_DESC& desc)
{
    auto reservation = reserve(1);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    createRenderTargetView(context, reservation.value(), 0, renderTargetResource, desc);

    return FixedDescriptorHeapDescriptor{std::move(reservation.value())};
}

FixedDescriptorHeap_DSV::FixedDescriptorHeap_DSV(DeviceContext& context, uint32_t descriptorCount)
    : FixedDescriptorHeapAllocator(context, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, descriptorCount)
{}

void FixedDescriptorHeap_DSV::createDepthStencilView(DeviceContext& context,
                                                     const FixedDescriptorHeapReservation& reservation,
                                                     uint32_t allocationIndex,
                                                     ID3D12Resource* depthStencilResource,
                                                     const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
{
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createdepthstencilview
    context.getDevice()->CreateDepthStencilView(depthStencilResource, &desc, reservation.getCpuHandle(allocationIndex));
}

tl::expected<FixedDescriptorHeapDescriptor, FreeBlockTracker::Error>
FixedDescriptorHeap_DSV::createDepthStencilView(DeviceContext& context,
                                                ID3D12Resource* depthStencilResource,
                                                const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
{
    auto reservation = reserve(1);
    if(!reservation) { return tl::make_unexpected(reservation.error()); }

    createDepthStencilView(context, reservation.value(), 0, depthStencilResource, desc);

    return FixedDescriptorHeapDescriptor{std::move(reservation.value())};
}
} // namespace scrap::d3d12