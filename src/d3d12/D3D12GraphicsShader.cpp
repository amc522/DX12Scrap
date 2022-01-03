#include "d3d12/D3D12GraphicsShader.h"

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12ShaderReflection.h"
#include "d3d12/D3D12Strings.h"

#include <charconv>

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
        for(GraphicsShaderStage stage : enumerate<GraphicsShaderStage>())
        {
            if(!mParams.filepaths[stage].empty() && !mParams.entryPoints[stage].empty())
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
    for(GraphicsShaderStage stage : enumerate(mParams.stages))
    {
        ShaderInfo& shaderInfo = mShaders[stage];

        EnumArray<const wchar_t*, GraphicsShaderStage> shaderTargets = {L"vs_6_6", L"hs_6_6", L"ds_6_6", L"gs_6_6",
                                                                        L"ps_6_6"};

        const std::string& entryPoint = mParams.entryPoints[stage];

        int wideStrSize = MultiByteToWideChar(CP_UTF8, 0, entryPoint.c_str(), (int)entryPoint.size(), nullptr, 0);
        std::wstring wideEntryPoint((size_t)wideStrSize, 0);
        MultiByteToWideChar(CP_UTF8, 0, entryPoint.c_str(), (int)entryPoint.size(), wideEntryPoint.data(),
                            (int)wideEntryPoint.size());

        LPCWSTR compilerArgs[] = {mParams.filepaths[stage].c_str(),
                                  L"-E",
                                  wideEntryPoint.c_str(),
                                  L"-T",
                                  shaderTargets[stage],
                                  (mParams.debug) ? L"-Zi" : L"",
                                  (mParams.debug) ? L"-Od" : L"O3"};

        // Load the source file
        const std::filesystem::path& filepath = mParams.filepaths[stage];
        ComPtr<IDxcBlobEncoding> sourceBlob;
        if(FAILED(utils->LoadFile(filepath.c_str(), nullptr, &sourceBlob)))
        {
            spdlog::error("Failed to load '{}' for shader compilation", filepath.generic_string());
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

            spdlog::error("Failed to compile shader '{}':\n{}", filepath.generic_string(), shaderInfo.compilerMessage);

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

        ComPtr<IDxcBlob> reflectionBlob = nullptr;
        hr = results->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr);

        if(FAILED(hr) || reflectionBlob == nullptr)
        {
            spdlog::error("Failed to get shader reflection data");
            continue;
        }

        DxcBuffer reflectionBuffer;
        reflectionBuffer.Encoding = DXC_CP_ACP;
        reflectionBuffer.Ptr = reflectionBlob->GetBufferPointer();
        reflectionBuffer.Size = reflectionBlob->GetBufferSize();

        ComPtr<ID3D12ShaderReflection> reflection;
        utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&reflection));

        UINT resourceIndex = 0;
        D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
        while(SUCCEEDED(reflection->GetResourceBindingDesc(resourceIndex++, &shaderInputBindDesc)))
        {
            if(shaderInputBindDesc.Type == D3D_SIT_TEXTURE || shaderInputBindDesc.Type == D3D_SIT_UAV_RWTYPED ||
               shaderInputBindDesc.Type == D3D_SIT_STRUCTURED || shaderInputBindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||
               shaderInputBindDesc.Type == D3D_SIT_BYTEADDRESS ||
               shaderInputBindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS ||
               shaderInputBindDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED ||
               shaderInputBindDesc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED ||
               shaderInputBindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER)
            {
                logShaderError(
                    stage,
                    "Global resource '{}' is not supported. Global resources (Texture, Buffer, etc.) are not "
                    "supported. Only ressources accessed through the ResourceDescriptorHeap array are supported.",
                    shaderInputBindDesc.Name);

                compiled = false;
                continue;
            }

            if(ShaderReflection::IsVertexIndicesRegister(shaderInputBindDesc))
            {
                tl::expected<VertexInputsCbInfo, ShaderReflectionErrorInfo> result =
                    ShaderReflection::CollectVertexInputs(reflection.Get(), shaderInputBindDesc);

                if(result) { mShaderInputs.vertexElements = std::move(result->vertexElements); }
                else
                {
                    switch(result.error().error)
                    {
                    case ShaderReflectionError::ResourceInReservedRegisterSpace:
                        logShaderError(
                            stage,
                            "Found {} resource '{}' at register {}, space {}. This is reserved for a constant "
                            "buffer that holds the vertex buffer descriptor indices.",
                            shaderInputBindDesc.Type, shaderInputBindDesc.Name, shaderInputBindDesc.BindPoint,
                            shaderInputBindDesc.Space);
                        break;
                    case ShaderReflectionError::GetConstantBufferFailure:
                        logShaderError(stage, "Failed to retrieve reflection data for constant buffer '{}'.",
                                       result.error().constantBufferName);
                        break;
                    case ShaderReflectionError::GetConstantBufferDescFailure:
                        logShaderError(stage, "Failed to retrieve reflection data for '{}' constant buffer.",

                                       result.error().constantBufferName);
                        break;
                    case ShaderReflectionError::InvalidMemberType:
                        logShaderError(stage,
                                       "Member '{}' in constant buffer '{}' must be a uint. This constant buffer is "
                                       "reserved for bindless vertex buffer indices.",
                                       ToHlslVariableString(result.error().variableType, result.error().memberName,
                                                            result.error().variableTypeRows,
                                                            result.error().variableTypeColumns,
                                                            result.error().variableElements),
                                       result.error().constantBufferName);
                        break;
                    case ShaderReflectionError::ExceededMaxResourceCount:
                        logShaderError(
                            stage,
                            "Exceeded the maximum allowed number of vertex buffers ({}). Found {} vertex buffers.",
                            d3d12::kMaxBindlessVertexBuffers, result.error().resourceOverflowCount);
                        break;
                    default: break;
                    }

                    continue;
                }
            }
            else if(ShaderReflection::IsResourceIndicesRegister(shaderInputBindDesc))
            {
                tl::expected<ResourcesCbInfo, ShaderReflectionErrorInfo> result =
                    ShaderReflection::CollectResourceInputs(reflection.Get(), shaderInputBindDesc);

                if(result) { mShaderInputs.resources = std::move(result->resources); }
                else
                {
                    switch(result.error().error)
                    {
                    case ShaderReflectionError::ResourceInReservedRegisterSpace:
                        logShaderError(
                            stage,
                            "Found {} resource '{}' at register {}, space {}. This is reserved for a constant "
                            "buffer that holds the resource descriptor indices.",
                            shaderInputBindDesc.Type, shaderInputBindDesc.Name, shaderInputBindDesc.BindPoint,
                            shaderInputBindDesc.Space);
                        break;
                    case ShaderReflectionError::GetConstantBufferFailure:
                        logShaderError(stage, "Failed to retrieve reflection data for constant buffer '{}'.",
                                       result.error().constantBufferName);
                        break;
                    case ShaderReflectionError::GetConstantBufferDescFailure:
                        logShaderError(stage, "Failed to retrieve reflection data for '{}' constant buffer.",

                                       result.error().constantBufferName);
                        break;
                    case ShaderReflectionError::InvalidMemberType:
                        logShaderError(stage,
                                       "Member '{}' in constant buffer '{}' must be a uint. This constant buffer is "
                                       "reserved for bindless resournce indices.",
                                       ToHlslVariableString(result.error().variableType, result.error().memberName,
                                                            result.error().variableTypeRows,
                                                            result.error().variableTypeColumns,
                                                            result.error().variableElements),
                                       result.error().constantBufferName);
                        break;
                    case ShaderReflectionError::ExceededMaxResourceCount:
                        logShaderError(stage,
                                       "Exceeded the maximum allowed number of resources ({}). Found {} resources.",
                                       d3d12::kMaxBindlessResources, result.error().resourceOverflowCount);
                        break;
                    default: break;
                    }

                    continue;
                }
            }
        }
    }

    mState = (compiled) ? GraphicsShaderState::Compiled : GraphicsShaderState::Failed;
}

D3D12_SHADER_BYTECODE GraphicsShader::getShaderByteCode(GraphicsShaderStage stage) const
{
    ID3DBlob* blob = mShaders[stage].shaderBlob.Get();
    if(blob != nullptr) { return CD3DX12_SHADER_BYTECODE(blob); }

    return {};
}

std::optional<uint32_t> GraphicsShader::getVertexElementIndex(ShaderVertexSemantic semantic,
                                                              uint32_t semanticIndex) const
{
    for(const ShaderVertexElement& element : mShaderInputs.vertexElements)
    {
        if(element.semantic == semantic && element.semanticIndex == semanticIndex) { return element.index; }
    }

    return std::nullopt;
}

std::optional<uint32_t> GraphicsShader::getResourceIndex(SharedString name,
                                                         ShaderResourceType resourceType,
                                                         ShaderResourceDimension resourceDimension) const
{
    for(const ShaderResource& resource : mShaderInputs.resources)
    {
        if(resource.name == name && resource.type == resourceType && resource.dimension == resourceDimension)
        {
            return resource.index;
        }
    }

    return std::nullopt;
}

void GraphicsShader::logShaderErrorImpl(GraphicsShaderStage stage, std::string_view message)
{
    spdlog::error("{}({}): {}", mParams.filepaths[stage].generic_string(), stage, message);
}
} // namespace scrap::d3d12