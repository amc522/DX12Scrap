#pragma once

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12Debug.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12FrameCodes.h"

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <d3d12.h>
#include <spdlog/spdlog.h>
#include <wrl/client.h>

namespace scrap::d3d12
{
template<class FrameCodeT>
class BaseCommandContext
{
public:
    BaseCommandContext(std::string_view debugName): mDebugName(debugName) {}

    virtual ~BaseCommandContext()
    {
        waitOnGpu();
        if(mFenceEvent != nullptr) { CloseHandle(mFenceEvent); }
    }

    void queueObjectForDestruction(Microsoft::WRL::ComPtr<ID3D12DeviceChild> deviceChild, FrameCodeT lastUsedFrameCode)
    {
        queueObjectForDestruction(std::move(deviceChild), FixedDescriptorHeapReservation{}, lastUsedFrameCode);
    }

    void queueObjectForDestruction(FixedDescriptorHeapReservation&& descriptors, FrameCodeT lastUsedFrameCode)
    {
        queueObjectForDestruction(nullptr, std::move(descriptors), lastUsedFrameCode);
    }

    void queueObjectForDestruction(Microsoft::WRL::ComPtr<ID3D12DeviceChild> deviceChild,
                                   FixedDescriptorHeapReservation&& descriptors,
                                   FrameCodeT lastUsedFrameCode)
    {
        if((deviceChild == nullptr && !descriptors.isValid()) || lastUsedFrameCode <= mLastCompletedFrameCode)
        {
            return;
        }

        std::lock_guard lockGuard(mPendingFreeListMutex);
        mPendingFreeList.emplace_back(std::move(deviceChild), std::move(descriptors), lastUsedFrameCode);
    }

    void queueObjectForDestruction(Microsoft::WRL::ComPtr<ID3D12DeviceChild> deviceChild,
                                   std::span<FixedDescriptorHeapReservation> descriptors,
                                   FrameCodeT lastUsedFrameCode)
    {
        if((deviceChild == nullptr && descriptors.empty()) || lastUsedFrameCode <= mLastCompletedFrameCode) { return; }

        std::lock_guard lockGuard(mPendingFreeListMutex);
        mPendingFreeList.emplace_back(std::move(deviceChild), std::move(descriptors.front()), lastUsedFrameCode);

        for(auto itr = descriptors.begin() + 1; itr != descriptors.end(); ++itr)
        {
            if(itr->isValid()) { mPendingFreeList.emplace_back(nullptr, std::move(*itr), lastUsedFrameCode); }
        }
    }

    virtual void beginFrame()
    {
        Debug::instance().beginGpuEvent(mCommandQueue.Get(), "{} Frame {}", mDebugName,
                                        (uint64_t)mFenceValues[mFrameIndex]);
    }

    virtual void endFrame()
    {
        // Schedule a Signal command in the queue.
        const UINT64 currentFenceValue = *mFenceValues[mFrameIndex];
        if(FAILED(mCommandQueue->Signal(mFence.Get(), currentFenceValue)))
        {
            spdlog::error("Failed to signal command queue;");
            return;
        }

        // Update the frame index.
        mFrameIndex = (mFrameIndex + 1u) % kFrameBufferCount;

        // If the next frame is not ready to be rendered yet, wait until it is ready.
        if(mFence->GetCompletedValue() < *mFenceValues[mFrameIndex])
        {
            if(FAILED(mFence->SetEventOnCompletion(*mFenceValues[mFrameIndex], mFenceEvent)))
            {
                spdlog::error("Fence SetEventOnCompletion call failed");
                return;
            }

            WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
        }

        mLastCompletedFrameCode = mFenceValues[mFrameIndex];

        // Set the fence value for the next frame.
        mFenceValues[mFrameIndex] = currentFenceValue + 1;

        { // Free all resources that are no longer being used by the gpu
            std::lock_guard lockGuard(mPendingFreeListMutex);
            mPendingFreeList.erase(std::remove_if(mPendingFreeList.begin(), mPendingFreeList.end(),
                                                  [&](const PendingFreeObject& resource) {
                                                      return resource.lastUsedFrameCode <= mLastCompletedFrameCode;
                                                  }),
                                   mPendingFreeList.end());
        }

        Debug::instance().endGpuEvent(mCommandQueue.Get());
    }

    void waitOnGpu()
    {
        if(mFence == nullptr) { return; }

        const uint64_t fenceValue = *mFenceValues[mFrameIndex];

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12commandqueue-signal
        if(FAILED(mCommandQueue->Signal(mFence.Get(), fenceValue)))
        {
            spdlog::error("Failed to update fence.");
            return;
        }

        if(FAILED(mFence->SetEventOnCompletion(fenceValue, mFenceEvent)))
        {
            spdlog::error("Fence SetEventOnCompletion call failed");
            return;
        }

        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    ID3D12CommandQueue* getCommandQueue() const { return mCommandQueue.Get(); }
    FrameCodeT getCurrentFrameCode() const { return mFenceValues[mFrameIndex]; }
    FrameCodeT getLastCompletedFrameCode() const { return mLastCompletedFrameCode; }

protected:
    HRESULT initInternal(ID3D12Device* device)
    {
        // Create synchronization objects and wait until assets have been uploaded to the GPU.
        HRESULT hr = device->CreateFence(*mFenceValues[mFrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
        if(FAILED(hr))
        {
            spdlog::error("Failed to create fence for command context");
            return hr;
        }

        ++mFenceValues[mFrameIndex];

        // Create an event handle to use for frame synchronization.
        // https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventw
        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if(mFenceEvent == nullptr)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            spdlog::critical("Failed to create fence event");
            return hr;
        }

        return S_OK;
    }

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

    uint32_t mFrameIndex = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    std::array<FrameCodeT, kFrameBufferCount> mFenceValues;
    HANDLE mFenceEvent = nullptr;

    FrameCodeT mLastCompletedFrameCode;

    std::string mDebugName;

    struct PendingFreeObject
    {
        PendingFreeObject(Microsoft::WRL::ComPtr<ID3D12DeviceChild>&& deviceChild_,
                          FixedDescriptorHeapReservation&& descriptors_,
                          FrameCodeT lastUsedFrameCode_)
            : deviceChild(std::move(deviceChild_))
            , descriptors(std::move(descriptors_))
            , lastUsedFrameCode(lastUsedFrameCode_)
        {}

        PendingFreeObject(const PendingFreeObject&) = delete;
        PendingFreeObject(PendingFreeObject&&) = default;
        ~PendingFreeObject() = default;

        PendingFreeObject& operator=(const PendingFreeObject&) = delete;
        PendingFreeObject& operator=(PendingFreeObject&&) = default;

        Microsoft::WRL::ComPtr<ID3D12DeviceChild> deviceChild;
        FixedDescriptorHeapReservation descriptors;
        FrameCodeT lastUsedFrameCode;
    };

    std::mutex mPendingFreeListMutex;
    std::vector<PendingFreeObject> mPendingFreeList;
};
} // namespace scrap::d3d12