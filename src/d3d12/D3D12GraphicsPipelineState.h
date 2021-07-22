#pragma once

#include "d3d12/D3D12FrameCodes.h"

#include <memory>
#include <mutex>
#include <vector>

#include <d3d12.h>
#include <dxgi.h>
#include <wrl/client.h>

namespace scrap::d3d12
{
class GraphicsShader;

struct GraphicsPipelineStateParams
{
    ID3D12RootSignature* rootSignature;
    std::shared_ptr<GraphicsShader> shader;
    std::vector<DXGI_FORMAT> renderTargetFormats; // replace with a fixed vector at some point
    DXGI_FORMAT depthStencilFormat;
    D3D12_RASTERIZER_DESC rasterizerState;
    D3D12_DEPTH_STENCIL_DESC depthStencilState;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
    D3D12_BLEND_DESC blendState;
};

class GraphicsPipelineState
{
public:
    GraphicsPipelineState() = default;
    GraphicsPipelineState(GraphicsPipelineStateParams&& params);
    GraphicsPipelineState(const GraphicsPipelineState&) = delete;
    GraphicsPipelineState(GraphicsPipelineState&& other) = default;
    ~GraphicsPipelineState();

    GraphicsPipelineState& operator=(const GraphicsPipelineState&) = delete;
    GraphicsPipelineState& operator=(GraphicsPipelineState&& other) = default;

    void create();
    bool isReady() const { return mPipelineState != nullptr; }

    ID3D12PipelineState* getPipelineState() const { return mPipelineState.Get(); }

    void markAsUsed();

private:
    GraphicsPipelineStateParams mParams;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
    std::mutex mCreationMutex;
    RenderFrameCode mLastUsedFrameCode;
};
} // namespace scrap::d3d12