#pragma once

#include "D3D12RaytracingShader.h"
#include "EASTL/fixed_vector.h"
#include "RenderDefs.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <memory>
#include <span>
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
    std::span<Microsoft::WRL::ComPtr<ID3D12RootSignature>> localRootSignatures;

    std::span<std::shared_ptr<RaytracingShader>> shaders;
    struct
    {
        RaytracingPipelineStateShaderParams raygen;
        RaytracingPipelineStateShaderParams intersection;
        RaytracingPipelineStateShaderParams anyHit;
        RaytracingPipelineStateShaderParams closestHit;
        RaytracingPipelineStateShaderParams miss;
    } fixedStages;

    std::span<RaytracingPipelineStateShaderParams> callables;

    std::string_view hitGroupName;

    RaytracingPipelineStatePrimitiveType primitiveType = RaytracingPipelineStatePrimitiveType::Triangles;

    uint32_t maxRecursionDepth = 1;
};

namespace detail
{
static constexpr uint32_t kShaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

} // namespace detail

using RaytracingShaderIdentifier = std::span<const std::byte, detail::kShaderIdentifierSize>;

namespace detail
{
static constexpr std::array<std::byte, kShaderIdentifierSize> kEmptyShaderIdentifierArray = {};
static constexpr RaytracingShaderIdentifier kEmptyShaderIdentifier(kEmptyShaderIdentifierArray);
} // namespace detail

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
    bool isReady() const { return true; }

    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

    [[nodiscard]] RaytracingPipelineStageMask getPipelineStages() const { return mPipelineStages; }

    [[nodiscard]] size_t getCallableShaderCount() const { return mCallableShaderParams.size(); }

    void setShaderIdentifier(RaytracingPipelineStage stage, const RaytracingShaderIdentifier& shaderIdentifier);
    void setCallableShaderIdentifier(size_t index, const RaytracingShaderIdentifier& shaderIdentifier);
    [[nodiscard]] RaytracingShaderIdentifier getShaderIdentifier(RaytracingPipelineStage stage) const;
    [[nodiscard]] RaytracingShaderIdentifier getCallableShaderIdentifier(size_t index) const
    {
        return mCallableShaderParams[index].shaderIdentifier;
    }

    [[nodiscard]] std::span<const D3D12_STATE_SUBOBJECT> getStateSubObjects() const { return mSubObjects; }

    std::wstring_view getFixedStageEntryPointName(RaytracingPipelineStage stage) const;
    std::wstring_view getCallableShaderEntryPointName(size_t index) const;

private:
    struct CallableShaderParams : RaytracingPipelineStateShaderParams
    {
        CallableShaderParams(const RaytracingPipelineStateShaderParams& other)
            : RaytracingPipelineStateShaderParams(other)
        {}

        RaytracingShaderIdentifier shaderIdentifier = detail::kEmptyShaderIdentifier;
    };

    std::wstring_view getFixedStageShaderEntryPointName(const RaytracingShaderStage stage) const;
    std::wstring_view getShaderEntryPointName(const RaytracingPipelineStateShaderParams& shaderParams) const;

    std::vector<std::shared_ptr<RaytracingShader>> mShaders;
    std::array<RaytracingPipelineStateShaderParams, (size_t)RaytracingShaderStage::Count> mFixedStageShaderParams;
    std::vector<CallableShaderParams> mCallableShaderParams;
    uint32_t mValidFixedStageExportCount = 0;
    std::wstring mHitGroupName;
    RaytracingPipelineStatePrimitiveType mPrimitiveType;
    uint32_t mMaxRecursionDepth = 1;
    RaytracingShaderStageMask mShaderStages = RaytracingShaderStageMask::None;
    RaytracingPipelineStageMask mPipelineStages = RaytracingPipelineStageMask::None;

    eastl::fixed_vector<TrackedGpuObject<ID3D12RootSignature>, 1> mLocalRootSignatures;

    std::array<RaytracingShaderIdentifier, (size_t)RaytracingPipelineStage::Count> mFixedStageShaderIdentifiers = {
        detail::kEmptyShaderIdentifier, detail::kEmptyShaderIdentifier, detail::kEmptyShaderIdentifier};

    MonotonicBuffer mMonotonicBuffer;
    std::span<D3D12_STATE_SUBOBJECT> mSubObjects;
};
} // namespace scrap::d3d12