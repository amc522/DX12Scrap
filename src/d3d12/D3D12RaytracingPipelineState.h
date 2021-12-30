#pragma once

#include "D3D12RaytracingShader.h"
#include "EASTL/fixed_vector.h"
#include "RenderDefs.h"
#include "d3d12/D3D12Fwd.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <memory>
#include <span>

#include <d3d12.h>
#include <wrl/client.h>

namespace scrap::d3d12
{
enum class RaytracingPipelineStatePrimitiveType
{
    Procedural,
    Triangles,
};

struct RaytracingPipelineStateShaderParams
{
    static constexpr size_t kInvalidIndex = std::numeric_limits<size_t>::max();

    size_t shaderIndex = kInvalidIndex;
    size_t shaderEntryPointIndex = kInvalidIndex;

    size_t localRootSignatureIndex = kInvalidIndex;

    constexpr bool hasValidShaderEntryPoint() const
    {
        return shaderIndex != kInvalidIndex && shaderEntryPointIndex != kInvalidIndex;
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

class RaytracingShaderIdentifier
{
public:
    static constexpr uint32_t kByteSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

    RaytracingShaderIdentifier() = default;
    RaytracingShaderIdentifier(const RaytracingShaderIdentifier&) = default;
    RaytracingShaderIdentifier(RaytracingShaderIdentifier&&) = default;
    RaytracingShaderIdentifier(std::span<const std::byte, kByteSize> bytes)
    {
        std::copy(bytes.begin(), bytes.end(), mBytes.begin());
    }

    RaytracingShaderIdentifier& operator=(const RaytracingShaderIdentifier&) = default;
    RaytracingShaderIdentifier& operator=(RaytracingShaderIdentifier&&) = default;

    [[nodiscard]] constexpr bool operator==(const RaytracingShaderIdentifier& right) const
    {
        return std::ranges::equal(mBytes, right.mBytes);
    }

    [[nodiscard]] std::array<std::byte, kByteSize>::const_iterator begin() const { return mBytes.cbegin(); }
    [[nodiscard]] std::array<std::byte, kByteSize>::const_iterator end() const { return mBytes.cend(); }

    [[nodiscard]] std::span<const std::byte> getBytes() const { return mBytes; }

    [[nodiscard]] constexpr bool isInvalid() const
    {
        return !std::all_of(mBytes.begin(), mBytes.end(), [](std::byte val) { return val == std::byte(0); });
    }

private:
    std::array<std::byte, kByteSize> mBytes{};
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

        RaytracingShaderIdentifier shaderIdentifier;
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

    std::array<RaytracingShaderIdentifier, (size_t)RaytracingPipelineStage::Count> mFixedStageShaderIdentifiers{};

    MonotonicBuffer mMonotonicBuffer;
    std::span<D3D12_STATE_SUBOBJECT> mSubObjects;
};
} // namespace scrap::d3d12
