#pragma once

#include "RenderDefs.h"
#include "Utility.h"

#include <array>
#include <filesystem>
#include <mutex>

#include <wrl/client.h>

struct D3D12_SHADER_BYTECODE;
struct ID3D10Blob;
using ID3DBlob = ID3D10Blob;

namespace scrap::d3d12
{
struct GraphicsShaderParams
{
    std::array<std::filesystem::path, (size_t)GraphicsShaderStage::Count> filepaths;
    std::array<std::string, (size_t)GraphicsShaderStage::Count> entryPoints;
    GraphicsShaderStageMask stages = GraphicsShaderStageMask::None;
    bool debug = false;
};

enum class GraphicsShaderState
{
    Invalid,
    Initialized,
    Compiling,
    Failed,
    Compiled
};

class GraphicsShader
{
public:
    GraphicsShader() = default;
    GraphicsShader(GraphicsShaderParams&& params);
    GraphicsShader(const GraphicsShader&) = delete;
    GraphicsShader(GraphicsShader&& other) noexcept;
    ~GraphicsShader() = default;

    GraphicsShader& operator=(const GraphicsShader&) = delete;
    GraphicsShader& operator=(GraphicsShader&& other) noexcept;

    void create();

    GraphicsShaderState status() const { return mState; }

    bool hasShaderStage(GraphicsShaderStage stage)
    {
        auto stageMask = GraphicsShaderStageToMask(stage);
        return (stageMask & mParams.stages) == stageMask;
    }

    ID3DBlob* getShader(GraphicsShaderStage stage) const { return mShaders[(size_t)stage].shaderBlob.Get(); }
    D3D12_SHADER_BYTECODE getShaderByteCode(GraphicsShaderStage stage) const;

private:
    struct ShaderInfo
    {
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        HRESULT hr = S_FALSE;
        std::string compilerMessage;
    };

    GraphicsShaderParams mParams;
    std::array<ShaderInfo, (size_t)GraphicsShaderStage::Count> mShaders;
    GraphicsShaderState mState = GraphicsShaderState::Invalid;
    std::mutex mCreationMutex;
};
} // namespace scrap::d3d12