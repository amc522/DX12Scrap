#include "D3D12RaytracingShader.h"

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12ShaderReflection.h"
#include "d3d12/D3D12Strings.h"

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
    : mState(RaytracingShaderState::Initialized)
    , mFilepath(std::move(params.filepath))
    , mDebug(params.debug)
{
    mFixedStageShaders.reserve(params.fixedStageEntryPoints.size());

    for(const RaytracingFixedStageShaderEntryPoint& entryPoint : params.fixedStageEntryPoints)
    {
        mAvailableStages |= RaytracingShaderStageToMask(entryPoint.stage);

        RaytracingFixedStageShaderInfo shaderInfo;
        shaderInfo.stage = entryPoint.stage;
        shaderInfo.entryPoint = entryPoint.entryPoint;
        shaderInfo.wideEntryPoint = Utf8ToWideSharedString(shaderInfo.entryPoint);

        mFixedStageShaders.push_back(std::move(shaderInfo));
    }

    std::stable_sort(mFixedStageShaders.begin(), mFixedStageShaders.end(),
                     [](const RaytracingFixedStageShaderInfo& left, const RaytracingFixedStageShaderInfo& right) {
                         return left.stage < right.stage;
                     });

    mCallableShaders.reserve(params.callableEntryPoints.size());

    for(const RaytracingCallableShaderEntryPoint& entryPoint : params.callableEntryPoints)
    {
        RaytracingCallableShaderInfo shaderInfo;
        shaderInfo.entryPoint = entryPoint.entryPoint;
        shaderInfo.wideEntryPoint = Utf8ToWideSharedString(shaderInfo.entryPoint);

        mCallableShaders.push_back(std::move(shaderInfo));
    }
}

void RaytracingShader::create()
{
    std::lock_guard lockGuard(mCreationMutex);

    if(mState != RaytracingShaderState::Initialized) { return; }

    mState = RaytracingShaderState::Compiling;

    if(mFixedStageShaders.empty() && mCallableShaders.empty())
    {
        mState = RaytracingShaderState::Failed;
        return;
    }

    ComPtr<IDxcUtils> utils;
    ComPtr<IDxcCompiler3> compiler;
    if(FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))
    {
        spdlog::error("Failed to create IDxcUtils when compiling raytracing shader {}.", mFilepath.generic_string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    if(FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))))
    {
        spdlog::error("Failed to create IDxcCompiler3 when compiling raytracing shader {}.",
                      mFilepath.generic_string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    ComPtr<IDxcIncludeHandler> includeHandler;
    if(FAILED(utils->CreateDefaultIncludeHandler(&includeHandler)))
    {
        spdlog::error("Failed to create default include handler when compiling raytracing shader {}.",
                      mFilepath.generic_string());
    }

    UINT compileFlags = 0;

    LPCWSTR compilerArgs[] = {mFilepath.c_str(), L"-T lib_6_6", (mDebug) ? L"-Zi" : L"", (mDebug) ? L"-Od" : L"O3", L"-enable-16bit-types"};

    ComPtr<IDxcBlobEncoding> sourceBlob;
    if(FAILED(utils->LoadFile(mFilepath.c_str(), nullptr, &sourceBlob)))
    {
        spdlog::error("Failed to load '{}' for raytracing shader compilation.", mFilepath.generic_string());
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

        spdlog::error("Failed to compile raytracing shader '{}':\n{}", mFilepath.generic_string(), mCompilerMessage);

        mState = RaytracingShaderState::Failed;
        return;
    }

    ComPtr<IDxcBlob> shader = nullptr;
    ComPtr<IDxcBlobUtf16> shaderName = nullptr;
    hr = results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), shaderName.GetAddressOf());

    if(FAILED(hr) || shader == nullptr)
    {
        spdlog::error("Failed to get raytracing shader binary for '{}'.", mFilepath.generic_string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    if(FAILED(shader.As(&mShaderBlob)))
    {
        spdlog::error("Failed to get raytracing shader binary for '{}'.", mFilepath.generic_string());
        mState = RaytracingShaderState::Failed;
        return;
    }

    ComPtr<IDxcBlob> reflectionBlob = nullptr;
    hr = results->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr);

    DxcBuffer reflectionBuffer;
    reflectionBuffer.Encoding = DXC_CP_ACP;
    reflectionBuffer.Ptr = reflectionBlob->GetBufferPointer();
    reflectionBuffer.Size = reflectionBlob->GetBufferSize();

    ComPtr<ID3D12LibraryReflection> reflection;
    hr = utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&reflection));

    D3D12_LIBRARY_DESC libraryDesc;
    reflection->GetDesc(&libraryDesc);

    for(RaytracingFixedStageShaderInfo& shaderInfo : mFixedStageShaders)
    {
        bool found = false;

        for(uint32_t i = 0; i < libraryDesc.FunctionCount; ++i)
        {
            ID3D12FunctionReflection* functionReflection = reflection->GetFunctionByIndex(i);

            D3D12_FUNCTION_DESC functionDesc;
            functionReflection->GetDesc(&functionDesc);

            std::string_view functionName(functionDesc.Name, std::strlen(functionDesc.Name));

            auto itr = std::search(functionName.begin(), functionName.end(), shaderInfo.entryPoint.begin(),
                                   shaderInfo.entryPoint.end());

            if(itr == functionName.end()) { continue; }

            found = true;

            for(uint32_t resourceIndex = 0; resourceIndex < functionDesc.BoundResources; ++resourceIndex)
            {
                D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
                HRESULT hr = functionReflection->GetResourceBindingDesc(resourceIndex, &shaderInputBindDesc);

                if(FAILED(hr)) { continue; }

                if(ShaderReflection::IsVertexIndicesRegister(shaderInputBindDesc))
                {
                    ID3D12ShaderReflectionConstantBuffer* constantBufferReflection =
                        functionReflection->GetConstantBufferByName(shaderInputBindDesc.Name);
                    auto result = ShaderReflection::CollectVertexInputs(constantBufferReflection);

                    if(result) { shaderInfo.inputs.vertexElements = std::move(result->vertexElements); }
                    else
                    {
                        switch(result.error().error)
                        {
                        case ShaderReflectionError::ResourceInReservedRegisterSpace:
                            logShaderError(
                                functionDesc.Name,
                                "Found {} resource '{}' at register {}, space {}. This is reserved for a constant "
                                "buffer that holds the vertex buffer descriptor indices.",
                                shaderInputBindDesc.Type, shaderInputBindDesc.Name, shaderInputBindDesc.BindPoint,
                                shaderInputBindDesc.Space);
                            break;
                        case ShaderReflectionError::GetConstantBufferFailure:
                            logShaderError(functionDesc.Name,
                                           "Failed to retrieve reflection data for constant buffer '{}'.",
                                           result.error().constantBufferName);
                            break;
                        case ShaderReflectionError::GetConstantBufferDescFailure:
                            logShaderError(functionDesc.Name,
                                           "Failed to retrieve reflection data for '{}' constant buffer.",

                                           result.error().constantBufferName);
                            break;
                        case ShaderReflectionError::InvalidMemberType:
                            logShaderError(
                                functionDesc.Name,
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
                                functionDesc.Name,
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
                    ID3D12ShaderReflectionConstantBuffer* constantBufferReflection =
                        functionReflection->GetConstantBufferByName(shaderInputBindDesc.Name);
                    auto result = ShaderReflection::CollectResourceInputs(constantBufferReflection);

                    if(result) { shaderInfo.inputs.resources = std::move(result->resources); }
                    else
                    {
                        switch(result.error().error)
                        {
                        case ShaderReflectionError::ResourceInReservedRegisterSpace:
                            logShaderError(
                                functionDesc.Name,
                                "Found {} resource '{}' at register {}, space {}. This is reserved for a constant "
                                "buffer that holds the resource descriptor indices.",
                                shaderInputBindDesc.Type, shaderInputBindDesc.Name, shaderInputBindDesc.BindPoint,
                                shaderInputBindDesc.Space);
                            break;
                        case ShaderReflectionError::GetConstantBufferFailure:
                            logShaderError(functionDesc.Name,
                                           "Failed to retrieve reflection data for constant buffer '{}'.",
                                           result.error().constantBufferName);
                            break;
                        case ShaderReflectionError::GetConstantBufferDescFailure:
                            logShaderError(functionDesc.Name,
                                           "Failed to retrieve reflection data for '{}' constant buffer.",
                                           result.error().constantBufferName);
                            break;
                        case ShaderReflectionError::InvalidMemberType:
                            logShaderError(
                                functionDesc.Name,
                                "Member '{}' in constant buffer '{}' must be a uint. This constant buffer is "
                                "reserved for bindless resournce indices.",
                                ToHlslVariableString(result.error().variableType, result.error().memberName,
                                                     result.error().variableTypeRows,
                                                     result.error().variableTypeColumns,
                                                     result.error().variableElements),
                                result.error().constantBufferName);
                            break;
                        case ShaderReflectionError::ExceededMaxResourceCount:
                            logShaderError(functionDesc.Name,
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

        if(!found)
        {
            spdlog::error("Failed to find '{}' when compiling '{}'", shaderInfo.entryPoint, mFilepath.generic_string());
            mState = RaytracingShaderState::Failed;
            return;
        }
    }

    mState = RaytracingShaderState::Compiled;
}

D3D12_SHADER_BYTECODE RaytracingShader::getShaderByteCode() const
{
    return CD3DX12_SHADER_BYTECODE(mShaderBlob.Get());
}

std::span<const RaytracingFixedStageShaderInfo>
RaytracingShader::getFixedStageShaders(RaytracingShaderStage stage) const
{
    auto stageStartItr =
        std::find_if(mFixedStageShaders.cbegin(), mFixedStageShaders.cend(),
                     [stage](const RaytracingFixedStageShaderInfo& info) { return info.stage == stage; });

    if(stageStartItr == mFixedStageShaders.cend()) { return {}; }

    auto stageEndItr = stageStartItr + 1;
    while(stageEndItr != mFixedStageShaders.cend() && stageEndItr->stage == stage)
    {
        ++stageEndItr;
    }

    return std::span(stageStartItr, stageEndItr);
}

const RaytracingFixedStageShaderInfo* RaytracingShader::getFixedStageShader(RaytracingShaderStage stage,
                                                                                          size_t index) const
{
    auto shaders = getFixedStageShaders(stage);

    if(index >= shaders.size()) { return nullptr; }

    return &shaders[index];
}

size_t RaytracingShader::getFixedStageIndex(RaytracingShaderStage stage, std::string_view entryPointName) const
{
    auto itr = std::find_if(mFixedStageShaders.cbegin(), mFixedStageShaders.cend(),
                            [stage, entryPointName](const auto& stageShader) {
                                return stageShader.stage == stage && stageShader.entryPoint == entryPointName;
                            });
    return std::distance(mFixedStageShaders.begin(), itr);
}

size_t RaytracingShader::getFixedStageIndex(RaytracingShaderStage stage, std::wstring_view entryPointName) const
{
    auto itr = std::find_if(mFixedStageShaders.cbegin(), mFixedStageShaders.cend(),
                            [stage, entryPointName](const auto& stageShader) {
                                return stageShader.stage == stage && stageShader.wideEntryPoint == entryPointName;
                            });
    return std::distance(mFixedStageShaders.begin(), itr);
}

size_t RaytracingShader::getFixedStageIndex(RaytracingShaderStage stage, StringHash entryPointName) const
{
    auto itr = std::find_if(mFixedStageShaders.cbegin(), mFixedStageShaders.cend(),
                            [stage, entryPointName](const auto& stageShader) {
                                return stageShader.stage == stage && stageShader.entryPoint == entryPointName;
                            });
    return std::distance(mFixedStageShaders.begin(), itr);
}

size_t RaytracingShader::getFixedStageIndex(RaytracingShaderStage stage, WStringHash entryPointName) const
{
    auto itr = std::find_if(mFixedStageShaders.cbegin(), mFixedStageShaders.cend(),
                            [stage, entryPointName](const auto& stageShader) {
                                return stageShader.stage == stage && stageShader.wideEntryPoint == entryPointName;
                            });
    return std::distance(mFixedStageShaders.cbegin(), itr);
}

size_t RaytracingShader::getFixedStageIndex(RaytracingShaderStage stage, const SharedString& entryPointName) const
{
    return getFixedStageIndex(stage, entryPointName.hash());
}

size_t RaytracingShader::getFixedStageIndex(RaytracingShaderStage stage, const WSharedString& entryPointName) const
{
    return getFixedStageIndex(stage, entryPointName.hash());
}

std::optional<uint32_t> RaytracingShader::getVertexElementIndex(RaytracingShaderStage stage, ShaderVertexSemantic semantic, uint32_t semanticIndex) const
{
    const RaytracingFixedStageShaderInfo* shaderInfo = getFixedStageShader(stage, 0);

    if(shaderInfo == nullptr) { return std::nullopt; }

    for(const ShaderVertexElement& element : shaderInfo->inputs.vertexElements)
    {
        if(element.semantic == semantic && element.semanticIndex == semanticIndex) { return element.index; }
    }

    return std::nullopt;
}

std::optional<uint32_t> RaytracingShader::getResourceIndex(RaytracingShaderStage stage, uint64_t nameHash, ShaderResourceType resourceType, ShaderResourceDimension resourceDimension) const
{
    const RaytracingFixedStageShaderInfo* shaderInfo = getFixedStageShader(stage, 0);

    if(shaderInfo == nullptr) { return std::nullopt; }

    for(const ShaderResource& resource : shaderInfo->inputs.resources)
    {
        if(resource.nameHash == resource.nameHash && resource.type == resourceType &&
           resource.dimension == resourceDimension)
        {
            return resource.index;
        }
    }

    return std::nullopt;
}

void RaytracingShader::logShaderErrorImpl(std::string_view stageName, std::string_view message)
{
    spdlog::error("{} ({}): {}", mFilepath.generic_string(), stageName, message);
}

detail::RaytracingShaderInfo* RaytracingShader::findShaderInfoByName(std::string_view name)
{
    return nullptr;
}
} // namespace scrap::d3d12