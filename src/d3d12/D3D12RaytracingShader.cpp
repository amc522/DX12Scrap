#include "D3D12RaytracingShader.h"

#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcommon.h>
#include <d3dx12.h>
#include <dxcapi.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
RaytracingShader::RaytracingShader(RaytracingShaderParams&& params)
    : mParams(std::move(params))
    , mState(RaytracingShaderState::Initialized)
{}

void RaytracingShader::create()
{
    std::lock_guard lockGuard(mCreationMutex);

    if(mState != RaytracingShaderState::Initialized) { return; }

    mState = RaytracingShaderState::Compiling;

    if(mParams.stages != RaytracingShaderStageMask::RgChMs && mParams.stages != RaytracingShaderStageMask::RgIsAhChMs)
    {
        mState = RaytracingShaderState::Failed;
        return;
    }

    ComPtr<IDxcUtils> utils;
    ComPtr<IDxcCompiler3> compiler;
    if(FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))
    {
        spdlog::error("Failed to create IDxcUtils when compiling raytracing shader {}.",
                      mParams.filepath.generic_u8string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    if(FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))))
    {
        spdlog::error("Failed to create IDxcCompiler3 when compiling raytracing shader {}.",
                      mParams.filepath.generic_u8string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    ComPtr<IDxcIncludeHandler> includeHandler;
    if(FAILED(utils->CreateDefaultIncludeHandler(&includeHandler)))
    {
        spdlog::error("Failed to create default include handler when compiling raytracing shader {}.",
                      mParams.filepath.generic_u8string());
    }

    UINT compileFlags = 0;

    LPCWSTR compilerArgs[] = {mParams.filepath.c_str(), L"-T lib_6_6", (mParams.debug) ? L"-Zi" : L"",
                              (mParams.debug) ? L"-Od" : L"O3"};

    ComPtr<IDxcBlobEncoding> sourceBlob;
    if(FAILED(utils->LoadFile(mParams.filepath.c_str(), nullptr, &sourceBlob)))
    {
        spdlog::error("Failed to load '{}' for raytracing shader compilation.", mParams.filepath.generic_u8string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    DxcBuffer source;
    source.Ptr = sourceBlob->GetBufferPointer();
    source.Size = sourceBlob->GetBufferSize();
    source.Encoding = DXC_CP_ACP;

    ComPtr<IDxcResult> results;
    HRESULT hr =
        compiler->Compile(&source, compilerArgs, _countof(compilerArgs), includeHandler.Get(), IID_PPV_ARGS(&results));

    ComPtr<IDxcBlobUtf8> errors;
    results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);

    if(FAILED(hr) || (errors != nullptr && errors->GetStringLength() != 0))
    {
        mCompilerMessage = std::string((const char*)errors->GetStringPointer(), errors->GetStringLength());

        spdlog::error("Failed to compile raytracing shader '{}':\n{}", mParams.filepath.generic_u8string(),
                      mCompilerMessage);

        mState = RaytracingShaderState::Failed;
        return;
    }

    ComPtr<IDxcBlob> shader = nullptr;
    ComPtr<IDxcBlobUtf16> shaderName = nullptr;
    hr = results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), shaderName.GetAddressOf());

    if(FAILED(hr) || shader == nullptr)
    {
        spdlog::error("Failed to get raytracing shader binary for '{}'.", mParams.filepath.generic_u8string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    if(FAILED(shader.As(&mShaderBlob)))
    {
        spdlog::error("Failed to get raytracing shader binary for '{}'.", mParams.filepath.generic_u8string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    mState = RaytracingShaderState::Compiled;
}

D3D12_SHADER_BYTECODE RaytracingShader::getShaderByteCode() const
{
    return CD3DX12_SHADER_BYTECODE(mShaderBlob.Get());
}
} // namespace scrap::d3d12