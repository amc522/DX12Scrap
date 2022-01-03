#pragma once

#include "D3D12RaytracingShader.h"
#include "EASTL/fixed_vector.h"
#include "EnumArray.h"
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
    Microsoft::WRL::ComPtr<ID3D12RootSignature> localRootSignature;
    size_t shaderEntryPointIndex = kInvalidIndex;

    constexpr bool hasValidShaderEntryPoint() const { return shaderEntryPointIndex != kInvalidIndex; }
};

struct RaytracingPipelineStateParams
{
    std::shared_ptr<RaytracingShader> shader;
    EnumArray<std::optional<RaytracingPipelineStateShaderParams>, RaytracingShaderStage> fixedStages;
    std::span<RaytracingPipelineStateShaderParams> callables;
    std::string_view hitGroupName;
    RaytracingPipelineStatePrimitiveType primitiveType = RaytracingPipelineStatePrimitiveType::Triangles;
    uint32_t maxRecursionDepth = 1;
};

class RaytracingShaderIdentifier
{
public:
    static constexpr uint32_t kByteSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

    constexpr RaytracingShaderIdentifier() = default;
    constexpr RaytracingShaderIdentifier(const RaytracingShaderIdentifier&) = default;
    constexpr RaytracingShaderIdentifier(RaytracingShaderIdentifier&&) = default;
    constexpr RaytracingShaderIdentifier(std::span<const std::byte, kByteSize> bytes)
    {
        std::copy(bytes.begin(), bytes.end(), mBytes.begin());
    }

    constexpr RaytracingShaderIdentifier& operator=(const RaytracingShaderIdentifier&) = default;
    constexpr RaytracingShaderIdentifier& operator=(RaytracingShaderIdentifier&&) = default;

    [[nodiscard]] constexpr bool operator==(const RaytracingShaderIdentifier& right) const
    {
        return std::ranges::equal(mBytes, right.mBytes);
    }

    [[nodiscard]] std::array<std::byte, kByteSize>::const_iterator begin() const { return mBytes.cbegin(); }
    [[nodiscard]] std::array<std::byte, kByteSize>::const_iterator end() const { return mBytes.cend(); }

    [[nodiscard]] constexpr std::span<const std::byte> getBytes() const { return mBytes; }

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

    const std::shared_ptr<RaytracingShader>& getShader() const { return mShader; }

    [[nodiscard]] RaytracingShaderStageMask getShaderStages() const { return mShaderStages; }
    [[nodiscard]] RaytracingPipelineStageMask getPipelineStages() const { return mPipelineStages; }

    [[nodiscard]] size_t getCallableShaderCount() const { return mCallableShaderParams.size(); }

    void setShaderIdentifier(RaytracingPipelineStage stage, const RaytracingShaderIdentifier& shaderIdentifier);
    void setCallableShaderIdentifier(size_t index, const RaytracingShaderIdentifier& shaderIdentifier);
    [[nodiscard]] const RaytracingShaderIdentifier& getShaderIdentifier(RaytracingPipelineStage stage) const;
    [[nodiscard]] const RaytracingShaderIdentifier& getCallableShaderIdentifier(size_t index) const
    {
        return mCallableShaderParams[index].shaderIdentifier;
    }

    [[nodiscard]] std::span<const D3D12_STATE_SUBOBJECT> getStateSubObjects() const { return mSubObjects; }

    std::wstring_view getShaderEntryPointName(RaytracingShaderStage stage) const;
    std::wstring_view getShaderEntryPointName(RaytracingPipelineStage stage) const;
    std::wstring_view getCallableShaderEntryPointName(size_t index) const;

private:
    struct ShaderStageParams
    {
        size_t shaderEntryPointIndex = std::numeric_limits<size_t>::max();
        size_t localRootSignatureIndex = std::numeric_limits<size_t>::max();
        RaytracingShaderIdentifier shaderIdentifier;
    };

    std::wstring_view getFixedStageShaderEntryPointName(const ShaderStageParams& shaderParams) const;

    static constexpr RaytracingShaderIdentifier kInvalidShaderIdentifier{};

    std::shared_ptr<RaytracingShader> mShader;
    eastl::fixed_vector<TrackedGpuObject<ID3D12RootSignature>, 1> mLocalRootSignatures;

    EnumArray<ShaderStageParams, RaytracingShaderStage> mFixedStageShaderParams;
    std::vector<ShaderStageParams> mCallableShaderParams;
    std::wstring mHitGroupName;
    RaytracingPipelineStatePrimitiveType mPrimitiveType;
    uint32_t mMaxRecursionDepth = 1;

    uint32_t mValidFixedStageExportCount = 0;
    RaytracingShaderStageMask mShaderStages = RaytracingShaderStageMask::None;
    RaytracingPipelineStageMask mPipelineStages = RaytracingPipelineStageMask::None;

    MonotonicBuffer mMonotonicBuffer;
    std::span<D3D12_STATE_SUBOBJECT> mSubObjects;

    RaytracingShaderIdentifier mHitGroupShaderIdentifier;
};
} // namespace scrap::d3d12
