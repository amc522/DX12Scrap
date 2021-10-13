#pragma once

#include "RenderDefs.h"

#include <array>
#include <filesystem>
#include <mutex>
#include <optional>

#include <wrl/client.h>

struct D3D12_SHADER_BYTECODE;
struct ID3D10Blob;
using ID3DBlob = ID3D10Blob;

namespace scrap::d3d12
{
struct RaytracingShaderParams
{
    // std::array<std::filesystem::path, (size_t)RaytracingShaderStage::Count> filepaths;
    // std::array<std::string, (size_t)RaytracingShaderStage::Count> entryPoints;
    std::filesystem::path filepath;
    RaytracingShaderStageMask stages = RaytracingShaderStageMask::None;
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
        return (stageMask & mParams.stages) == stageMask;
    }

    ID3DBlob* getShader() const { return mShaderBlob.Get(); }
    D3D12_SHADER_BYTECODE getShaderByteCode() const;

    /* ShaderInputs& accessShaderInputs() { return mShaderInputs; }
       const ShaderInputs& getShaderInputs() const { return mShaderInputs; }

       std::optional<uint32_t> getVertexElementIndex(ShaderVertexSemantic semantic, uint32_t semanticIndex) const;
       std::optional<uint32_t> getResourceIndex(uint64_t nameHash, ShaderResourceType resourceType,
       ShaderResourceDimension resourceDimension) const;*/

private:
    struct ShaderInfo
    {};

    RaytracingShaderParams mParams;
    std::array<ShaderInfo, (size_t)GraphicsShaderStage::Count> mShaders;
    RaytracingShaderState mState = RaytracingShaderState::Invalid;
    std::mutex mCreationMutex;

    Microsoft::WRL::ComPtr<ID3DBlob> mShaderBlob;
    std::string mCompilerMessage;
};
} // namespace scrap::d3d12