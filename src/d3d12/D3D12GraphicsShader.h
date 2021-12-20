#pragma once

#include "RenderDefs.h"
#include "Utility.h"

#include <array>
#include <filesystem>
#include <mutex>
#include <optional>
#include <span>

#include <wrl/client.h>

struct D3D12_SHADER_BYTECODE;
struct _D3D12_SHADER_INPUT_BIND_DESC;
using D3D12_SHADER_INPUT_BIND_DESC = _D3D12_SHADER_INPUT_BIND_DESC;
struct ID3D10Blob;
using ID3DBlob = ID3D10Blob;
struct ID3D12ShaderReflection;

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

    ShaderInputs& accessShaderInputs() { return mShaderInputs; }
    const ShaderInputs& getShaderInputs() const { return mShaderInputs; }

    std::optional<uint32_t> getVertexElementIndex(ShaderVertexSemantic semantic, uint32_t semanticIndex) const;
    std::optional<uint32_t> getResourceIndex(uint64_t nameHash,
                                             ShaderResourceType resourceType,
                                             ShaderResourceDimension resourceDimension) const;

private:
    struct ShaderInfo
    {
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        HRESULT hr = S_FALSE;
        std::string compilerMessage;
    };

    template<class... Args>
    void logShaderError(GraphicsShaderStage stage, std::string_view messageFormatStr, Args&&... messageArgs)
    {
        std::string message = fmt::format(messageFormatStr, std::forward<Args>(messageArgs)...);
        logShaderErrorImpl(stage, message);
    }

    void logShaderErrorImpl(GraphicsShaderStage stage, std::string_view message);

    GraphicsShaderParams mParams;
    std::array<ShaderInfo, (size_t)GraphicsShaderStage::Count> mShaders;
    GraphicsShaderState mState = GraphicsShaderState::Invalid;
    std::mutex mCreationMutex;

    bool mVertexConstantBufferFound = false;
    ShaderInputs mShaderInputs;
};
} // namespace scrap::d3d12