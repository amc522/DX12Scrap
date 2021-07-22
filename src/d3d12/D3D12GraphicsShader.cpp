#include "d3d12/D3D12GraphicsShader.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
GraphicsShader::GraphicsShader(GraphicsShaderParams&& params)
    : mParams(std::move(params))
    , mState(GraphicsShaderState::Initialized)
{}

GraphicsShader::GraphicsShader(GraphicsShader&& other) noexcept
    : mParams(std::move(other.mParams))
    , mShaders(std::move(other.mShaders))
    , mState(other.mState)
{
    other.mState = GraphicsShaderState::Invalid;
}

GraphicsShader& GraphicsShader::operator=(GraphicsShader&& other) noexcept
{
    mParams = std::move(other.mParams);
    mShaders = std::move(other.mShaders);
    mState = other.mState;
    other.mState = GraphicsShaderState::Invalid;

    return *this;
}

void GraphicsShader::create()
{
    std::lock_guard lockGuard(mCreationMutex);

    if(mState != GraphicsShaderState::Initialized) { return; }

    mState = GraphicsShaderState::Compiling;

    if(mParams.stages == GraphicsShaderStageMask::None)
    {
        for(GraphicsShaderStage stage = GraphicsShaderStage::First; stage <= GraphicsShaderStage::Last;
            stage = IncrementEnum(stage))
        {
            if(!mParams.filepaths[(size_t)stage].empty() && !mParams.entryPoints[(size_t)stage].empty())
            {
                mParams.stages |= GraphicsShaderStageToMask(stage);
            }
        }
    }

    if(mParams.stages != GraphicsShaderStageMask::Vertex && mParams.stages != GraphicsShaderStageMask::VsGs &&
       mParams.stages != GraphicsShaderStageMask::VsPs && mParams.stages != GraphicsShaderStageMask::VsHsDs &&
       mParams.stages != GraphicsShaderStageMask::VsGsPs && mParams.stages != GraphicsShaderStageMask::VsHsDsGs &&
       mParams.stages != GraphicsShaderStageMask::VsHsDsPs && mParams.stages != GraphicsShaderStageMask::VsHsDsGsPs)
    {
        mState = GraphicsShaderState::Failed;
        return;
    }

    UINT compileFlags = 0;

    if(mParams.debug) { compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION; }

    bool compiled = true;
    for(GraphicsShaderStage stage = GraphicsShaderStage::First; stage <= GraphicsShaderStage::Last;
        stage = IncrementEnum(stage))
    {
        if(!hasShaderStage(stage)) { continue; }

        ShaderInfo& shaderInfo = mShaders[(size_t)stage];

        ComPtr<ID3DBlob> error;

        std::array<const char*, (size_t)GraphicsShaderStage::Count> shaderTargets = {"vs_5_0", "hs_5_0", "ds_5_0",
                                                                                     "gs_5_0", "ps_5_0"};

        shaderInfo.hr = D3DCompileFromFile(mParams.filepaths[(size_t)stage].c_str(), nullptr, nullptr,
                                           mParams.entryPoints[(size_t)stage].c_str(), shaderTargets[(size_t)stage],
                                           compileFlags, 0, &shaderInfo.shaderBlob, &error);

        if(FAILED(shaderInfo.hr))
        {
            compiled = false;

            if(error != nullptr)
            {
                shaderInfo.compilerMessage =
                    std::string((const char*)error->GetBufferPointer(), error->GetBufferSize());
            }
        }
    }

    mState = (compiled) ? GraphicsShaderState::Compiled : GraphicsShaderState::Failed;
}

D3D12_SHADER_BYTECODE GraphicsShader::getShaderByteCode(GraphicsShaderStage stage) const
{
    ID3DBlob* blob = mShaders[(size_t)stage].shaderBlob.Get();
    if(blob != nullptr) { return CD3DX12_SHADER_BYTECODE(blob); }

    return {};
}
} // namespace scrap::d3d12