#pragma once

#include "EastlFixedVectorExt.h"
#include "RenderDefs.h"
#include "SharedString.h"

#include <filesystem>
#include <mutex>
#include <span>

#include <EASTL/fixed_vector.h>
#include <wrl/client.h>

struct D3D12_SHADER_BYTECODE;
struct ID3D10Blob;
using ID3DBlob = ID3D10Blob;

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
    SharedString entryPoint;
    WSharedString wideEntryPoint;
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
    std::span<const RaytracingFixedStageShaderEntryPoint> fixedStageEntryPoints;
    std::span<const RaytracingCallableShaderEntryPoint> callableEntryPoints;
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

    std::span<const RaytracingFixedStageShaderInfo, (size_t)RaytracingShaderStage::Count> getFixedStageShaders() const
    {
        return ToSpan(mFixedStageShaders);
    }
    std::span<const RaytracingCallableShaderInfo> getCallableShaders() const { return mCallableShaders; }

    /* ShaderInputs& accessShaderInputs() { return mShaderInputs; }
       const ShaderInputs& getShaderInputs() const { return mShaderInputs; }

       std::optional<uint32_t> getVertexElementIndex(ShaderVertexSemantic semantic, uint32_t semanticIndex) const;
       std::optional<uint32_t> getResourceIndex(uint64_t nameHash, ShaderResourceType resourceType,
       ShaderResourceDimension resourceDimension) const;*/

private:
    std::filesystem::path mFilepath;
    RaytracingShaderStageMask mAvailableStages = RaytracingShaderStageMask::None;

    eastl::fixed_vector<RaytracingFixedStageShaderInfo, (size_t)RaytracingShaderStage::Count> mFixedStageShaders;
    std::vector<RaytracingCallableShaderInfo> mCallableShaders;

    bool mDebug = false;
    RaytracingShaderState mState = RaytracingShaderState::Invalid;
    std::mutex mCreationMutex;

    Microsoft::WRL::ComPtr<ID3DBlob> mShaderBlob;
    std::string mCompilerMessage;
};
} // namespace scrap::d3d12