#include "d3d12/D3D12CopyContext.h"

#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12Debug.h"

#include <d3d12.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
HRESULT CopyContext::init()
{
    DeviceContext& deviceContext = DeviceContext::instance();

    BaseCommandContext<CopyFrameCode>::initInternal(deviceContext.instance().getDevice());

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

    HRESULT hr = deviceContext.getDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue));
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

    return S_OK;
}

void CopyContext::beginFrame()
{
    BaseCommandContext<CopyFrameCode>::beginFrame();

    ID3D12CommandAllocator* currentCommandAllocator = mCommandAllocators[mFrameIndex].Get();
    currentCommandAllocator->Reset();
    mCommandList->Reset(currentCommandAllocator, nullptr);
}

void CopyContext::endFrame()
{
    mCommandList->Close();

    const std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());

    BaseCommandContext<CopyFrameCode>::endFrame();
}

} // namespace scrap::d3d12