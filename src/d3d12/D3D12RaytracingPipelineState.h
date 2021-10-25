#pragma once

#include "D3D12RaytracingShader.h"
#include "EASTL/fixed_vector.h"
#include "RenderDefs.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <memory>
#include <variant>

#include <d3d12.h>
#include <wrl/client.h>

namespace scrap::d3d12
{
class RaytracingShader;

enum class RaytracingPipelineStatePrimitiveType
{
    Procedural,
    Triangles,
};

struct RaytracingPipelineStateShaderParams
{
    size_t shaderIndex = std::numeric_limits<size_t>::max();
    size_t shaderEntryPointIndex = std::numeric_limits<size_t>::max();

    size_t localRootSignatureIndex = std::numeric_limits<size_t>::max();

    constexpr bool hasValidShaderEntryPoint() const
    {
        return shaderIndex != std::numeric_limits<size_t>::max() &&
               shaderEntryPointIndex != std::numeric_limits<size_t>::max();
    }
};

struct RaytracingPipelineStateParams
{
    nonstd::span<Microsoft::WRL::ComPtr<ID3D12RootSignature>> localRootSignatures;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSignature;

    nonstd::span<std::shared_ptr<RaytracingShader>> shaders;
    struct
    {
        RaytracingPipelineStateShaderParams raygen;
        RaytracingPipelineStateShaderParams intersection;
        RaytracingPipelineStateShaderParams anyHit;
        RaytracingPipelineStateShaderParams closestHit;
        RaytracingPipelineStateShaderParams miss;
    } fixedStages;

    nonstd::span<RaytracingPipelineStateShaderParams> callables;

    std::string_view hitGroupName;

    RaytracingPipelineStatePrimitiveType primitiveType = RaytracingPipelineStatePrimitiveType::Triangles;

    uint32_t maxPayloadByteSize = 0;
    uint32_t maxAttributeByteSize = 0;
    uint32_t maxRecursionDepth = 1;
};

class RaytracingPipelineState
{
public:
    RaytracingPipelineState() = default;
    RaytracingPipelineState(RaytracingPipelineStateParams&& params);
    RaytracingPipelineState(const RaytracingPipelineState&) = delete;
    RaytracingPipelineState(RaytracingPipelineState&& other) = default;
    ~RaytracingPipelineState() = default;

    RaytracingPipelineState& operator=(const RaytracingPipelineState&) = delete;
    RaytracingPipelineState& operator=(RaytracingPipelineState&& other) = default;

    void create();
    bool isReady() const { return mStateObject != nullptr; }

    ID3D12StateObject* getPipelineState() const { return mStateObject.get(); }

    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

private:
    std::wstring_view getShaderEntryPointName(const RaytracingPipelineStateShaderParams& shaderParams) const;

    TrackedGpuObject<ID3D12StateObject> mStateObject;
    std::vector<std::shared_ptr<RaytracingShader>> mShaders;
    std::array<RaytracingPipelineStateShaderParams, (size_t)RaytracingShaderStage::Count> mFixedStageShaderParams;
    std::vector<RaytracingPipelineStateShaderParams> mCallableShaderParams;
    uint32_t mValidFixedStageExportCount = 0;
    std::wstring mHitGroupName;
    RaytracingPipelineStatePrimitiveType mPrimitiveType;
    uint32_t mMaxPayloadByteSize = 0;
    uint32_t mMaxAttributeByteSize = 0;
    uint32_t mMaxRecursionDepth = 1;

    TrackedGpuObject<ID3D12RootSignature> mGlobalRootSignature;
    eastl::fixed_vector<TrackedGpuObject<ID3D12RootSignature>, 1> mLocalRootSignatures;
};
} // namespace scrap::d3d12