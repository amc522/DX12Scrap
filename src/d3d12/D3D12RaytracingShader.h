#pragma once

#include "EastlFixedVectorExt.h"
#include "RenderDefs.h"
#include "SharedString.h"
#include "d3d12/D3D12Fwd.h"

#include <filesystem>
#include <mutex>
#include <span>

#include <EASTL/fixed_vector.h>
#include <wrl/client.h>

namespace scrap::d3d12
{
struct RaytracingFixedStageShaderEntryPoint
{
    RaytracingShaderStage stage = RaytracingShaderStage::None;
    SharedString entryPoint;
};

struct RaytracingCallableShaderEntryPoint
{
    SharedString entryPoint;
};

namespace detail
{
struct RaytracingShaderInfo
{
    size_t index = 0;
    SharedString entryPoint;
    WSharedString wideEntryPoint;
    ShaderInputs inputs;
};
} // namespace detail

struct RaytracingCallableShaderInfo : public detail::RaytracingShaderInfo
{};

struct RaytracingFixedStageShaderInfo : public detail::RaytracingShaderInfo
{
    RaytracingShaderStage stage = RaytracingShaderStage::None;
};

struct RaytracingShaderParams
{
    std::filesystem::path filepath;
    std::span<RaytracingFixedStageShaderEntryPoint> fixedStageEntryPoints;
    std::span<RaytracingCallableShaderEntryPoint> callableEntryPoints;
    bool debug = false;
};

enum class RaytracingShaderState
{
    Invalid,
    Initialized,
    Compiling,
    Failed,
    Compiled
};

class RaytracingShader
{
private:
public:
    RaytracingShader() = default;
    RaytracingShader(RaytracingShaderParams&& params);
    RaytracingShader(const RaytracingShader&) = delete;
    RaytracingShader(RaytracingShader&& other) noexcept = delete;
    ~RaytracingShader() = default;

    RaytracingShader& operator=(const RaytracingShader&) = delete;
    RaytracingShader& operator=(RaytracingShader&& other) noexcept = delete;

    void create();

    RaytracingShaderState status() const { return mState; }

    bool hasShaderStage(RaytracingShaderStage stage)
    {
        auto stageMask = RaytracingShaderStageToMask(stage);
        return (stageMask & mAvailableStages) == stageMask;
    }

    ID3DBlob* getShaderResource() const { return mShaderBlob.Get(); }
    D3D12_SHADER_BYTECODE getShaderByteCode() const;

    std::span<const RaytracingFixedStageShaderInfo> getFixedStageShaders() const { return ToSpan(mFixedStageShaders); }

    std::span<const RaytracingFixedStageShaderInfo> getFixedStageShaders(RaytracingShaderStage stage) const;
    const RaytracingFixedStageShaderInfo* getFixedStageShader(RaytracingShaderStage stage, size_t index) const;
    std::span<const RaytracingCallableShaderInfo> getCallableShaders() const { return mCallableShaders; }

    size_t getFixedStageIndex(RaytracingShaderStage stage, std::string_view entryPointName) const;
    size_t getFixedStageIndex(RaytracingShaderStage stage, std::wstring_view entryPointName) const;
    size_t getFixedStageIndex(RaytracingShaderStage stage, StringHash entryPointName) const;
    size_t getFixedStageIndex(RaytracingShaderStage stage, WStringHash entryPointName) const;
    size_t getFixedStageIndex(RaytracingShaderStage stage, const SharedString& entryPointName) const;
    size_t getFixedStageIndex(RaytracingShaderStage stage, const WSharedString& entryPointName) const;

    std::optional<uint32_t>
    getVertexElementIndex(RaytracingShaderStage stage, ShaderVertexSemantic semantic, uint32_t semanticIndex) const;
    std::optional<uint32_t> getResourceIndex(RaytracingShaderStage stage,
                                             SharedString name,
                                             ShaderResourceType resourceType,
                                             ShaderResourceDimension resourceDimension) const;

private:
    template<class... Args>
    void logShaderError(std::string_view stageName, std::string_view messageFormatStr, Args&&... messageArgs)
    {
        std::string message = fmt::format(messageFormatStr, std::forward<Args>(messageArgs)...);
        logShaderErrorImpl(stageName, message);
    }

    void logShaderErrorImpl(std::string_view stageName, std::string_view message);

    detail::RaytracingShaderInfo* findShaderInfoByName(std::string_view name);

    std::filesystem::path mFilepath;
    RaytracingShaderStageMask mAvailableStages = RaytracingShaderStageMask::None;

    using FixedStageShaderVector =
        eastl::fixed_vector<RaytracingFixedStageShaderInfo, (size_t)RaytracingShaderStage::Count>;
    FixedStageShaderVector mFixedStageShaders;
    std::vector<RaytracingCallableShaderInfo> mCallableShaders;

    bool mDebug = false;
    RaytracingShaderState mState = RaytracingShaderState::Invalid;
    std::mutex mCreationMutex;

    Microsoft::WRL::ComPtr<ID3DBlob> mShaderBlob;
    std::string mCompilerMessage;
};
} // namespace scrap::d3d12