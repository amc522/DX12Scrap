#include "d3d12/D3D12GraphicsPipelineState.h"

#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12GraphicsShader.h"

#include <chrono>

#include <d3dx12.h>
#include <spdlog/spdlog.h>

namespace scrap::d3d12
{
GraphicsPipelineState::GraphicsPipelineState(GraphicsPipelineStateParams&& params): mParams(std::move(params)) {}

GraphicsPipelineState::~GraphicsPipelineState()
{
    DeviceContext::instance().getGraphicsContext().queueObjectForDestruction(std::move(mPipelineState), mLastUsedFrameCode);
}

void GraphicsPipelineState::create()
{
    std::lock_guard lockGuard(mCreationMutex);

    if(mPipelineState != nullptr) { return; }

    if(mParams.shader->status() == GraphicsShaderState::Initialized) { mParams.shader->create(); }

    while(mParams.shader->status() == GraphicsShaderState::Compiling)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if(mParams.shader->status() != GraphicsShaderState::Compiled) { return; }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.pRootSignature = mParams.rootSignature;
    desc.VS = mParams.shader->getShaderByteCode(GraphicsShaderStage::Vertex);
    desc.HS = mParams.shader->getShaderByteCode(GraphicsShaderStage::Hull);
    desc.DS = mParams.shader->getShaderByteCode(GraphicsShaderStage::Domain);
    desc.GS = mParams.shader->getShaderByteCode(GraphicsShaderStage::Geometry);
    desc.PS = mParams.shader->getShaderByteCode(GraphicsShaderStage::Pixel);
    desc.BlendState = mParams.blendState;
    desc.RasterizerState = mParams.rasterizerState;
    desc.DepthStencilState = mParams.depthStencilState;
    desc.PrimitiveTopologyType = mParams.primitiveTopologyType;
    desc.NumRenderTargets = (UINT)mParams.renderTargetFormats.size();
    std::copy(mParams.renderTargetFormats.cbegin(), mParams.renderTargetFormats.cend(), desc.RTVFormats);
    desc.DSVFormat = mParams.depthStencilFormat;
    desc.SampleMask = UINT_MAX;
    desc.SampleDesc.Count = 1;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-creategraphicspipelinestate
    if(FAILED(DeviceContext::instance().getDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&mPipelineState))))
    {
        spdlog::critical("Failed to create d3d12 pipeline state object");
        return;
    }
}

void GraphicsPipelineState::markAsUsed()
{
    mLastUsedFrameCode = DeviceContext::instance().getGraphicsContext().getCurrentFrameCode();
}
} // namespace scrap::d3d12