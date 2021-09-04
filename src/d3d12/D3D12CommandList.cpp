#include "D3D12CommandList.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
GraphicsCommandList::GraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, std::string_view debugName)
{
    std::wstring wideName;
    int wideStrSize = MultiByteToWideChar(CP_UTF8, 0u, debugName.data(), (int)debugName.size(), nullptr, 0);
    wideName.resize(wideStrSize);
    MultiByteToWideChar(CP_UTF8, 0u, debugName.data(), (int)debugName.size(), wideName.data(), (int)wideName.size());

    DeviceContext& deviceContext = DeviceContext::instance();
    ID3D12Device* device = deviceContext.getDevice();

    for(size_t frameIndex = 0; frameIndex < d3d12::kFrameBufferCount; ++frameIndex)
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandallocator
        if(FAILED(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator))))
        {
            spdlog::critical("Failed to create d3d12 command allocator");
            return;
        }

        commandAllocator->SetName(wideName.c_str());
        mCommandAllocators[frameIndex] = TrackedGpuObject(std::move(commandAllocator));
    }

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandlist
    HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                           mCommandAllocators[deviceContext.getFrameIndex()].get(), nullptr,
                                           IID_PPV_ARGS(&mCommandList));

    if(FAILED(hr))
    {
        spdlog::critical("Failed to create d3d12 command list");
        return;
    }

    mCommandList->SetName(wideName.c_str());
}

HRESULT GraphicsCommandList::reset()
{
    mFrameIndex = (mFrameIndex + 1) % kFrameBufferCount;

    auto& currentCommandAllocator = mCommandAllocators[mFrameIndex];

    if(currentCommandAllocator.isInUse(mCommandList.Get()))
    {
        assert(false);
        return E_NOT_VALID_STATE;
    }

    // Command list allocators can only be reset when the associated
    // command lists have finished execution on the GPU; apps should use
    // fences to determine GPU execution progress.
    HRESULT hr = currentCommandAllocator->Reset();
    if(FAILED(hr))
    {
        return hr;
    }

    currentCommandAllocator.markAsUsed(mCommandList.Get());

    // However, when ExecuteCommandList() is called on a particular command
    // list, that command list can then be reset at any time and must be before
    // re-recording.
    hr = mCommandList->Reset(currentCommandAllocator.get(), nullptr);
    if(FAILED(hr))
    {
        return hr;
    }

    return S_OK;
}

HRESULT GraphicsCommandList::close()
{
    return mCommandList->Close();
}
} // namespace scrap::d3d12
