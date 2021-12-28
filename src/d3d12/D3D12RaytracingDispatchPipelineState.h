#pragma once

#include "d3d12/D3D12Fwd.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace scrap::d3d12
{
struct RaytracingDispatchPipelineStateParams
{
    Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSignature;

    uint32_t maxPayloadByteSize = 0;
    uint32_t maxAttributeByteSize = 0;
};

class RaytracingDispatchPipelineState
{
public:
    RaytracingDispatchPipelineState() = default;
    RaytracingDispatchPipelineState(const RaytracingDispatchPipelineState&) = delete;
    RaytracingDispatchPipelineState(RaytracingDispatchPipelineState&&) = default;
    ~RaytracingDispatchPipelineState() = default;

    RaytracingDispatchPipelineState& operator=(const RaytracingDispatchPipelineState&) = delete;
    RaytracingDispatchPipelineState& operator=(RaytracingDispatchPipelineState&&) = default;

    void init(RaytracingDispatchPipelineStateParams&& params);

    void addPipelineState(std::shared_ptr<RaytracingPipelineState> pipelineState);

    void markAsUsed(const GraphicsCommandList& commandList);

    ID3D12StateObject* getStateObject() const { return mStateObject.get(); }

private:
    std::vector<D3D12_STATE_SUBOBJECT> mTempStateSubObjects;
    TrackedGpuObject<ID3D12RootSignature> mGlobalRootSignature;
    TrackedGpuObject<ID3D12StateObject> mStateObject;
    uint32_t mMaxPayloadByteSize = 0;
    uint32_t mMaxAttributeByteSize = 0;
};
} // namespace scrap::d3d12