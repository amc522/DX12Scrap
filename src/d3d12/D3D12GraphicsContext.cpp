#include "d3d12/D3D12GraphicsContext.h"

#include "d3d12/D3D12Context.h"

namespace scrap::d3d12
{
GraphicsContext::GraphicsContext(): BaseCommandContext<RenderFrameCode>("Render") {}

HRESULT GraphicsContext::init()
{
    DeviceContext& deviceContext = DeviceContext::instance();

    HRESULT hr = BaseCommandContext<RenderFrameCode>::initInternal(deviceContext.instance().getDevice());

    if(FAILED(hr)) { return hr; }

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandqueue
    hr = DeviceContext::instance().getDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue));
    if(FAILED(hr))
    {
        spdlog::critical("Failed to create d3d12 direct command queue");
        return hr;
    }

    mCommandQueue->SetName(L"Render Command Queue");

    spdlog::info("Created d3d12 direct command queue");

    return S_OK;
}
} // namespace scrap::d3d12
