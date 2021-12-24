#include "D3D12CommandList.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
GraphicsCommandList::GraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, std::string_view debugName)
    : mCommandListType(type)
{
    int wideStrSize = MultiByteToWideChar(CP_UTF8, 0u, debugName.data(), (int)debugName.size(), nullptr, 0);
    mDebugNameBase.resize(wideStrSize);
    MultiByteToWideChar(CP_UTF8, 0u, debugName.data(), (int)debugName.size(), mDebugNameBase.data(),
                        (int)mDebugNameBase.size());

    DeviceContext& deviceContext = DeviceContext::instance();
    ID3D12Device* device = deviceContext.getDevice();

    for(uint32_t frameIndex = 0; frameIndex < d3d12::kFrameBufferCount; ++frameIndex)
    {
        appendNewAllocator(frameIndex);
    }

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandlist
    mFrameIndex = deviceContext.getFrameIndex();
    HRESULT hr = device->CreateCommandList(0, type, mCommandAllocators[mFrameIndex].front().get(), nullptr,
                                           IID_PPV_ARGS(&mCommandList));

    if(FAILED(hr))
    {
        spdlog::critical("Failed to create d3d12 command list");
        return;
    }

    mCommandList->SetName(mDebugNameBase.c_str());

    hr = mCommandList->QueryInterface(IID_PPV_ARGS(&mCommandList4));

    mCommandList->Close();
}

HRESULT GraphicsCommandList::beginRecording()
{
    auto& currentCommandAllocatorArray = mCommandAllocators[mFrameIndex];

    TrackedGpuObject<ID3D12CommandAllocator>* currentCommandAllocator = nullptr;
    for(TrackedGpuObject<ID3D12CommandAllocator>& commandAllocator : currentCommandAllocatorArray)
    {
        if(commandAllocator.isInUse(mCommandList.Get())) { continue; }

        currentCommandAllocator = &commandAllocator;
    }

    if(currentCommandAllocator == nullptr)
    {
        appendNewAllocator(mFrameIndex);
        currentCommandAllocator = &currentCommandAllocatorArray.back();
    }

    // Command list allocators can only be reset when the associated
    // command lists have finished execution on the GPU; apps should use
    // fences to determine GPU execution progress.
    HRESULT hr = (*currentCommandAllocator)->Reset();
    if(FAILED(hr)) { return hr; }

    currentCommandAllocator->markAsUsed(mCommandList.Get());

    // However, when ExecuteCommandList() is called on a particular command
    // list, that command list can then be reset at any time and must be before
    // re-recording.
    hr = mCommandList->Reset(currentCommandAllocator->get(), nullptr);
    if(FAILED(hr)) { return hr; }

    return S_OK;
}

HRESULT GraphicsCommandList::execute(ID3D12CommandQueue* commandQueue)
{
    HRESULT hr = mCommandList->Close();
    if(FAILED(hr)) { return hr; }

    std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.Get()};
    commandQueue->ExecuteCommandLists((UINT)commandLists.size(), commandLists.data());

    return S_OK;
}

void GraphicsCommandList::endFrame()
{
    mFrameIndex = (mFrameIndex + 1) % kFrameBufferCount;
}

void GraphicsCommandList::setBindlessResource(uint32_t constantBufferIndex, uint32_t descriptorHeapIndex)
{
    mBindlessChangeBegin = std::min(mBindlessChangeBegin, constantBufferIndex);
    mBindlessChangeEnd = std::max(mBindlessChangeEnd, constantBufferIndex + 1);

    mBindlessResourceIndices[constantBufferIndex] = descriptorHeapIndex;
}

void GraphicsCommandList::commitBindlessResources()
{
    if(mBindlessChangeBegin > mBindlessChangeEnd) { return; }

    mCommandList->SetGraphicsRoot32BitConstants(
        d3d12::RasterRootParamSlot::ResourceIndices, mBindlessChangeEnd - mBindlessChangeBegin,
        mBindlessResourceIndices.data() + mBindlessChangeBegin, mBindlessChangeBegin);

    mBindlessChangeBegin = d3d12::kMaxBindlessResources;
    mBindlessChangeEnd = 0;
}

HRESULT GraphicsCommandList::appendNewAllocator(uint32_t frameIndex)
{
    DeviceContext& deviceContext = DeviceContext::instance();
    ID3D12Device* device = deviceContext.getDevice();

    ComPtr<ID3D12CommandAllocator> commandAllocator;
    HRESULT hr = device->CreateCommandAllocator(mCommandListType, IID_PPV_ARGS(&commandAllocator));

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandallocator
    if(FAILED(hr))
    {
        spdlog::critical("Failed to create d3d12 command allocator");
        return hr;
    }

    mDebugNameBuffer.clear();
    fmt::format_to(std::back_inserter(mDebugNameBuffer), L"{} allocator {}[{}]", mDebugNameBase, frameIndex,
                   mCommandAllocators[frameIndex].size());
    commandAllocator->SetName(mDebugNameBuffer.c_str());

    mCommandAllocators[frameIndex].emplace_back(std::move(commandAllocator));

    return S_OK;
}
} // namespace scrap::d3d12
