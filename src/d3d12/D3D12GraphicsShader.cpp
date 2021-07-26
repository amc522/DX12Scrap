#include "d3d12/D3D12GraphicsShader.h"

#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dx12.h>
#include <dxcapi.h>
#include <spdlog/spdlog.h>

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

    // dxc shader compiling tutorial:
    // https://github.com/microsoft/DirectXShaderCompiler/wiki/Using-dxc.exe-and-dxcompiler.dll
    ComPtr<IDxcUtils> utils;
    ComPtr<IDxcCompiler3> compiler;
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

    ComPtr<IDxcIncludeHandler> includeHandler;
    utils->CreateDefaultIncludeHandler(&includeHandler);

    UINT compileFlags = 0;

    bool compiled = true;
    for(GraphicsShaderStage stage = GraphicsShaderStage::First; stage <= GraphicsShaderStage::Last;
        stage = IncrementEnum(stage))
    {
        if(!hasShaderStage(stage)) { continue; }

        ShaderInfo& shaderInfo = mShaders[(size_t)stage];

        std::array<const wchar_t*, (size_t)GraphicsShaderStage::Count> shaderTargets = {L"vs_6_6", L"hs_6_6", L"ds_6_6",
                                                                                        L"gs_6_6", L"ps_6_6"};

        const std::string& entryPoint = mParams.entryPoints[(size_t)stage];

        int wideStrSize = MultiByteToWideChar(CP_UTF8, 0, entryPoint.c_str(), (int)entryPoint.size(), nullptr, 0);
        std::wstring wideEntryPoint((size_t)wideStrSize, 0);
        MultiByteToWideChar(CP_UTF8, 0, entryPoint.c_str(), (int)entryPoint.size(), wideEntryPoint.data(),
                            (int)wideEntryPoint.size());

        LPCWSTR compilerArgs[] = {mParams.filepaths[(size_t)stage].c_str(),
                                  L"-E",
                                  wideEntryPoint.c_str(),
                                  L"-T",
                                  shaderTargets[(size_t)stage],
                                  (mParams.debug) ? L"-Zi" : L"",
                                  (mParams.debug) ? L"-Od" : L"O3"};

        // Load the source file
        const std::filesystem::path& filepath = mParams.filepaths[(size_t)stage];
        ComPtr<IDxcBlobEncoding> sourceBlob;
        if(FAILED(utils->LoadFile(filepath.c_str(), nullptr, &sourceBlob)))
        {
            spdlog::error("Failed to load '{}' for shader compilation", filepath.generic_u8string());
            compiled = false;
            continue;
        }

        DxcBuffer source;
        source.Ptr = sourceBlob->GetBufferPointer();
        source.Size = sourceBlob->GetBufferSize();
        source.Encoding = DXC_CP_ACP;

        // Compile
        ComPtr<IDxcResult> results;
        HRESULT hr = compiler->Compile(&source, compilerArgs, _countof(compilerArgs), includeHandler.Get(),
                                       IID_PPV_ARGS(&results));

        // Check for errors
        ComPtr<IDxcBlobUtf8> errors;
        results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);

        if(FAILED(hr) || (errors != nullptr && errors->GetStringLength() != 0))
        {
            shaderInfo.compilerMessage =
                std::string((const char*)errors->GetStringPointer(), errors->GetStringLength());

            spdlog::error("Failed to compile shader '{}':\n{}", filepath.generic_u8string(),
                          shaderInfo.compilerMessage);

            compiled = false;
            continue;
        }

        ComPtr<IDxcBlob> shader = nullptr;
        ComPtr<IDxcBlobUtf16> shaderName = nullptr;
        hr = results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), shaderName.GetAddressOf());

        if(FAILED(hr) || shader == nullptr)
        {
            spdlog::error("Failed to get shader binary");
            continue;
        }

        // Convert from an IDxcBlob to ID3DBlob
        if(FAILED(shader.As(&shaderInfo.shaderBlob)))
        {
            spdlog::error("Failed to convert IDxcBlob to ID3DBlob");
            continue;
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