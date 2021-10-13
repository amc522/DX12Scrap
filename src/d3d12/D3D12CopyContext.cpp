#include "d3d12/D3D12CopyContext.h"

#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12Debug.h"

#include <d3d12.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
CopyContext::CopyContext(): BaseCommandContext<CopyFrameCode>("Copy") {}

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

    mCommandQueue->SetName(L"Copy Command Queue");

    mCommandList = std::make_unique<GraphicsCommandList>(D3D12_COMMAND_LIST_TYPE_COPY, "Copy Command List");

    return S_OK;
}

void CopyContext::releaseResources()
{
    mCommandList = nullptr;
}

void CopyContext::beginFrame()
{
    BaseCommandContext<CopyFrameCode>::beginFrame();

    mCommandList->beginRecording();
}

void CopyContext::endFrame()
{
    mCommandList->execute(mCommandQueue.Get());

    BaseCommandContext<CopyFrameCode>::endFrame();
}

void CopyContext::execute()
{
    mCommandList->execute(mCommandQueue.Get());
    mCommandList->beginRecording();
}

} // namespace scrap::d3d12