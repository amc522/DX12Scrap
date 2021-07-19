#include "d3d12/D3D12CopyContext.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
CopyContext::~CopyContext()
{
    waitOnGpu();
    if(mFenceEvent != nullptr) { CloseHandle(mFenceEvent); }
}

HRESULT CopyContext::init(DeviceContext& deviceContext)
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

    HRESULT hr = deviceContext.getDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCopyQueue));
    if(FAILED(hr))
    {
        spdlog::error("Failed to create copy queue");
        return hr;
    }

    for(ComPtr<ID3D12CommandAllocator>& commandAllocator : mCommandAllocators)
    {
        hr = deviceContext.getDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY,
                                                               IID_PPV_ARGS(&commandAllocator));
        if(FAILED(hr)) { spdlog::error("Failed to create copy command allocators"); }
    }

    hr = deviceContext.getDevice()->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_COPY, mCommandAllocators[mFrameIndex].Get(), nullptr, IID_PPV_ARGS(&mCommandList));

    if(FAILED(hr))
    {
        spdlog::error("Failed to create copy command list");
        return hr;
    }

    hr = mCommandList->Close();
    if(FAILED(hr))
    {
        spdlog::error("Failed to close copy command list");
        return hr;
    }

    hr = deviceContext.getDevice()->CreateFence(*mFenceValues[mFrameIndex], D3D12_FENCE_FLAG_NONE,
                                                IID_PPV_ARGS(&mFence));
    if(FAILED(hr))
    {
        spdlog::error("Failed to create copy queue fence");
        return hr;
    }

    mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if(mFenceEvent == nullptr)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        spdlog::critical("Failed to create fence event");
    }

    ++mFenceValues[mFrameIndex];

    return S_OK;
}

ID3D12CommandQueue* CopyContext::getCommandQueue() const
{
    return mCopyQueue.Get();
}

ID3D12GraphicsCommandList* CopyContext::getCommandList() const
{
    return mCommandList.Get();
}

void CopyContext::trackCopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> sourceResource,
                                    Microsoft::WRL::ComPtr<ID3D12Resource> destResource)
{
    mPendingResources.emplace_back(std::move(sourceResource), std::move(destResource), mFenceValues[mFrameIndex]);
}

void CopyContext::beginCopyFrame()
{
    ID3D12CommandAllocator* currentCommandAllocator = mCommandAllocators[mFrameIndex].Get();
    currentCommandAllocator->Reset();
    mCommandList->Reset(currentCommandAllocator, nullptr);
}

void CopyContext::endCopyFrame()
{
    mCommandList->Close();

    const std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.Get()};
    mCopyQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());

    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = *mFenceValues[mFrameIndex];
    if(FAILED(mCopyQueue->Signal(mFence.Get(), currentFenceValue)))
    {
        spdlog::error("Failed to signal command queue;");
        return;
    }

    // Update the frame index.
    mFrameIndex = (mFrameIndex + 1u) % kFrameBufferCount;
    mLastCompletedFrameCode = mFence->GetCompletedValue();

    // Set the fence value for the next frame.
    mFenceValues[mFrameIndex] = currentFenceValue + 1;

    mPendingResources.erase(std::remove_if(mPendingResources.begin(), mPendingResources.end(),
                                           [&](const PendingResource& pendingResource) {
                                               return pendingResource.copyFrameCode <= mLastCompletedFrameCode;
                                           }),
                            mPendingResources.end());
}

void CopyContext::waitOnGpu()
{
    if(mFence == nullptr) { return; }

    const uint64_t fenceValue = *mFenceValues[mFrameIndex];

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12commandqueue-signal
    if(FAILED(mCopyQueue->Signal(mFence.Get(), fenceValue)))
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
} // namespace scrap::d3d12