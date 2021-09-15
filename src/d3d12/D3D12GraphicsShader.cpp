#include "d3d12/D3D12GraphicsShader.h"

#include "d3d12/D3D12Config.h"
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

constexpr bool IsVertexIndicesRegister(const D3D12_SHADER_INPUT_BIND_DESC& desc)
{
    return desc.Type == D3D_SIT_CBUFFER && desc.BindPoint == reservedShaderRegister::kVertexCB.shaderRegister &&
           desc.Space == reservedShaderRegister::kVertexCB.registerSpace;
}

constexpr bool IsResourceIndicesRegister(const D3D12_SHADER_INPUT_BIND_DESC& desc)
{
    return desc.Type == D3D_SIT_CBUFFER && desc.BindPoint == reservedShaderRegister::kResourceCB.shaderRegister &&
           desc.Space == reservedShaderRegister::kResourceCB.registerSpace;
}

ShaderResource ParseBindlessResourceName(std::string_view name,
                                         std::string_view prefix = "g",
                                         std::string_view suffix = "DescriptorIndex")
{
    ShaderResource resource;

    if(name.empty()) { return {}; }

    if(!StartsWith(name, prefix)) { return {}; }

    name.remove_prefix(prefix.size());

    if(!EndsWith(name, suffix)) { return {}; }

    name.remove_suffix(suffix.size());

    // get the resource type
    size_t nextSeparatorPos = name.find("_");
    if(nextSeparatorPos == std::string_view::npos) { return {}; }

    const std::string_view typeName = name.substr(0, nextSeparatorPos);
    name = name.substr(nextSeparatorPos + 1);

    if(typeName == "AppendStructuredBuffer")
    {
        resource.type = ShaderResourceType::AppendBuffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "Buffer")
    {
        resource.type = ShaderResourceType::Buffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "ByteAddressBuffer")
    {
        resource.type = ShaderResourceType::ByteAddressBuffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "ConsumeStructuredBuffer")
    {
        resource.type = ShaderResourceType::ConsumeBuffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "RWBuffer")
    {
        resource.type = ShaderResourceType::RwBuffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "RWByteAddressBuffer")
    {
        resource.type = ShaderResourceType::RwByteAddressBuffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "RWStructuredBuffer")
    {
        resource.type = ShaderResourceType::RwStructuredBuffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "RWTexture1D")
    {
        resource.type = ShaderResourceType::RwTexture;
        resource.dimension = ShaderResourceDimension::Texture1d;
    }
    else if(typeName == "RWTexture1DArray")
    {
        resource.type = ShaderResourceType::RwTexture;
        resource.dimension = ShaderResourceDimension::Texture1dArray;
    }
    else if(typeName == "RWTexture2D")
    {
        resource.type = ShaderResourceType::RwTexture;
        resource.dimension = ShaderResourceDimension::Texture2d;
    }
    else if(typeName == "RWTexture2DArray")
    {
        resource.type = ShaderResourceType::RwTexture;
        resource.dimension = ShaderResourceDimension::Texture2dArray;
    }
    else if(typeName == "RWTexture3D")
    {
        resource.type = ShaderResourceType::RwTexture;
        resource.dimension = ShaderResourceDimension::Texture3d;
    }
    else if(typeName == "StructuredBuffer")
    {
        resource.type = ShaderResourceType::StructuredBuffer;
        resource.dimension = ShaderResourceDimension::Buffer;
    }
    else if(typeName == "Texture1D")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::Texture1d;
    }
    else if(typeName == "Texture1DArray")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::Texture1dArray;
    }
    else if(typeName == "Texture2D")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::Texture2d;
    }
    else if(typeName == "Texture2DArray")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::Texture2dArray;
    }
    else if(typeName == "Texture2DMS")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::Texture2d;
        resource.multisample = true;
    }
    else if(typeName == "Texture2DMSArray")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::Texture2dArray;
        resource.multisample = true;
    }
    else if(typeName == "Texture3D")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::Texture3d;
    }
    else if(typeName == "TextureCube")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::TextureCube;
    }
    else if(typeName == "TextureCubeArray")
    {
        resource.type = ShaderResourceType::Texture;
        resource.dimension = ShaderResourceDimension::TextureCubeArray;
    }

    // get the resource type
    nextSeparatorPos = name.find("_");
    if(nextSeparatorPos == std::string_view::npos) { return {}; }

    std::string_view returnTypeStr = name.substr(0, nextSeparatorPos);

    const size_t componentCountPos = name.find_first_of("1234");

    std::string_view componentCountStr;
    if(componentCountPos != std::string_view::npos)
    {
        componentCountStr = returnTypeStr.substr(componentCountPos);
        returnTypeStr = returnTypeStr.substr(0, componentCountPos);

        std::from_chars(componentCountStr.data(), componentCountStr.data() + componentCountStr.size(),
                        resource.returnTypeComponentCount);
    }

    if(returnTypeStr == "unorm") { resource.returnType = ShaderResourceReturnType::Unorm; }
    else if(returnTypeStr == "snorm")
    {
        resource.returnType = ShaderResourceReturnType::Snorm;
    }
    else if(returnTypeStr == "int")
    {
        resource.returnType = ShaderResourceReturnType::Int;
    }
    else if(returnTypeStr == "uint")
    {
        resource.returnType = ShaderResourceReturnType::UInt;
    }
    else if(returnTypeStr == "float")
    {
        resource.returnType = ShaderResourceReturnType::Float;
    }
    else if(returnTypeStr == "double")
    {
        resource.returnType = ShaderResourceReturnType::Double;
    }

    name = name.substr(nextSeparatorPos + 1);
    resource.name = name;
    resource.nameHash = std::hash<std::string_view>()(resource.name);

    return resource;
}

ShaderVertexElement ParseBindlessVertexBufferName(std::string_view name,
                                                  std::string_view prefix = "g",
                                                  std::string_view suffix = "DescriptorIndex",
                                                  std::string_view vertexPrefix = "Vertex")
{
    ShaderVertexElement vertexElement{ParseBindlessResourceName(name, prefix, suffix)};

    if(vertexElement.name.empty()) { return {}; }

    name = vertexElement.name;

    if(!StartsWith(name, vertexPrefix)) { return {}; }

    name.remove_prefix(vertexPrefix.size());

    const size_t digitIndex = name.find_first_of("0123456789");

    if(digitIndex != std::string_view::npos)
    {
        const std::string_view semanticIndexStr = name.substr(digitIndex);
        name = name.substr(0, digitIndex);

        const std::from_chars_result result = std::from_chars(
            semanticIndexStr.data(), semanticIndexStr.data() + semanticIndexStr.size(), vertexElement.semanticIndex);

        if(result.ec != std::errc()) { return {}; }
    }
    else
    {
        vertexElement.semanticIndex = 0;
    }

    if(name == "Positions" || name == "Position") { vertexElement.semantic = ShaderVertexSemantic::Position; }
    else if(name == "Normals" || name == "Normal")
    {
        vertexElement.semantic = ShaderVertexSemantic::Normal;
    }
    else if(name == "Tangents" || name == "Tangent")
    {
        vertexElement.semantic = ShaderVertexSemantic::Tangent;
    }
    else if(name == "Binormals" || name == "Binormal")
    {
        vertexElement.semantic = ShaderVertexSemantic::Binormal;
    }
    else if(name == "TexCoords" || name == "TexCoord" || name == "Uvs" || name == "Uv")
    {
        vertexElement.semantic = ShaderVertexSemantic::TexCoord;
    }
    else if(name == "Colors" || name == "Color")
    {
        vertexElement.semantic = ShaderVertexSemantic::Color;
    }

    return vertexElement;
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
                spdlog::error(
                    "{}({}): Global resource '{}' is not supported. Global resources (Texture, Buffer, etc.) are not "
                    "supported. Only ressources accessed through the ResourceDescriptorHeap array are supported.",
                    filepath.generic_u8string(), stage, shaderInputBindDesc.Name);
                compiled = false;
                continue;
            }

            if(IsVertexIndicesRegister(shaderInputBindDesc))
            {
                if(shaderInputBindDesc.Type != D3D_SIT_CBUFFER)
                {
                    spdlog::error(
                        "{}({}): Found {} resource '{}' at register {}, space {}. This is reserved for a constant "
                        "buffer that holds the vertex buffer descriptor indices.",
                        filepath.generic_u8string(), stage, shaderInputBindDesc.Type, shaderInputBindDesc.Name,
                        shaderInputBindDesc.BindPoint, shaderInputBindDesc.Space);

                    continue;
                }

                collectVertexInputs(stage, reflection.Get(), shaderInputBindDesc);
            }
            else if(IsResourceIndicesRegister(shaderInputBindDesc))
            {
                if(shaderInputBindDesc.Type != D3D_SIT_CBUFFER)
                {
                    spdlog::error(
                        "{}({}): Found {} resource '{}' at register {}, space {}. This is reserved for a constant "
                        "buffer that holds resource descriptor indices.",
                        filepath.generic_u8string(), stage, shaderInputBindDesc.Type, shaderInputBindDesc.Name,
                        shaderInputBindDesc.BindPoint, shaderInputBindDesc.Space);

                    continue;
                }

                collectResourceInputs(stage, reflection.Get(), shaderInputBindDesc);
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

std::optional<uint32_t> GraphicsShader::getVertexElementIndex(ShaderVertexSemantic semantic,
                                                              uint32_t semanticIndex) const
{
    for(const ShaderVertexElement& element : mShaderInputs.vertexElements)
    {
        if(element.semantic == semantic && element.semanticIndex == semanticIndex) { return element.index; }
    }

    return std::nullopt;
}

std::optional<uint32_t> GraphicsShader::getResourceIndex(uint64_t nameHash,
                                                         ShaderResourceType resourceType,
                                                         ShaderResourceDimension resourceDimension) const
{
    for(const ShaderResource& resource : mShaderInputs.resources)
    {
        if(resource.nameHash == resource.nameHash && resource.type == resourceType &&
           resource.dimension == resourceDimension)
        {
            return resource.index;
        }
    }

    return std::nullopt;
}

void GraphicsShader::collectVertexInputs(GraphicsShaderStage stage,
                                         ID3D12ShaderReflection* reflection,
                                         const D3D12_SHADER_INPUT_BIND_DESC& shaderInputBindDesc)
{
    ID3D12ShaderReflectionConstantBuffer* constantBufferReflection =
        reflection->GetConstantBufferByIndex(shaderInputBindDesc.uID);

    if(constantBufferReflection == nullptr)
    {
        spdlog::error("{}({}): Failed to retrieve reflection data for '{}' constant buffer.",
                      mParams.filepaths[(size_t)stage].generic_u8string(), stage, shaderInputBindDesc.Name);
        return;
    }

    D3D12_SHADER_BUFFER_DESC vertexCbDesc;
    if(FAILED(constantBufferReflection->GetDesc(&vertexCbDesc)))
    {
        spdlog::error("{}({}): Failed to retrieve reflection data for '{}' constant buffer.",
                      mParams.filepaths[(size_t)stage].generic_u8string(), stage, shaderInputBindDesc.Name);
        return;
    }

    mVertexConstantBufferFound = true;

    if(vertexCbDesc.Variables > d3d12::kMaxBindlessVertexBuffers)
    {
        spdlog::error("{}({}): Exceeded the maximum allowed number of vertex buffers ({}). Found {} vertex buffers.",
                      mParams.filepaths[(size_t)stage].generic_u8string(), stage, d3d12::kMaxBindlessVertexBuffers,
                      vertexCbDesc.Variables);
        return;
    }

    mShaderInputs.vertexElements.reserve(vertexCbDesc.Variables);

    for(uint32_t variableIndex = 0; variableIndex < vertexCbDesc.Variables; ++variableIndex)
    {
        ID3D12ShaderReflectionVariable* variable = constantBufferReflection->GetVariableByIndex(variableIndex);

        D3D12_SHADER_VARIABLE_DESC variableDesc;
        variable->GetDesc(&variableDesc);

        ID3D12ShaderReflectionType* type = variable->GetType();

        D3D12_SHADER_TYPE_DESC variableTypeDesc;
        if(FAILED(type->GetDesc(&variableTypeDesc)))
        {
            spdlog::error("{}({}): Failed to get type information for variable '{}' in constant buffer '{}'.",
                          mParams.filepaths[(size_t)stage].generic_u8string(), stage, variableDesc.Name,
                          vertexCbDesc.Name);
            continue;
        }

        if(variableTypeDesc.Class != D3D_SVC_SCALAR && variableTypeDesc.Type != D3D_SVT_UINT)
        {
            spdlog::error("{}({}): '{}' in cbuffer '{}' must be a uint.",
                          mParams.filepaths[(size_t)stage].generic_u8string(), stage, variableDesc.Name,
                          vertexCbDesc.Name);
            continue;
        }

        ShaderVertexElement vertexLayoutElement{ParseBindlessVertexBufferName(variableDesc.Name)};

        if(vertexLayoutElement.type != ShaderResourceType::Buffer)
        {
            spdlog::error(
                "{}({}): '{}' in cbuffer '{}' must be of the form gBuffer_[ReturnType]_Vertex[Semantic]DescriptorIndex",
                mParams.filepaths[(size_t)stage].generic_u8string(), stage, variableDesc.Name, vertexCbDesc.Name);
            continue;
        }

        vertexLayoutElement.index = variableDesc.StartOffset / sizeof(uint32_t);

        mShaderInputs.vertexElements.push_back(std::move(vertexLayoutElement));
    }
}

void GraphicsShader::collectResourceInputs(GraphicsShaderStage stage,
                                           ID3D12ShaderReflection* reflection,
                                           const D3D12_SHADER_INPUT_BIND_DESC& shaderInputBindDesc)
{
    ID3D12ShaderReflectionConstantBuffer* constantBufferReflection =
        reflection->GetConstantBufferByIndex(shaderInputBindDesc.uID);

    if(constantBufferReflection == nullptr)
    {
        spdlog::error("{}({}): Failed to retrieve reflection data for '{}' constant buffer.",
                      mParams.filepaths[(size_t)stage].generic_u8string(), stage, shaderInputBindDesc.Name);
        return;
    }

    D3D12_SHADER_BUFFER_DESC resourceCbDesc;
    if(FAILED(constantBufferReflection->GetDesc(&resourceCbDesc)))
    {
        spdlog::error("{}({}): Failed to retrieve reflection data for '{}' constant buffer.",
                      mParams.filepaths[(size_t)stage].generic_u8string(), stage, shaderInputBindDesc.Name);
        return;
    }

    if(resourceCbDesc.Variables > d3d12::kMaxBindlessResources)
    {
        spdlog::error("{}({}): Exceeded the maximum allowed number of bindless resources ({}). Found {} "
                      "bindless resources.",
                      mParams.filepaths[(size_t)stage].generic_u8string(), stage, d3d12::kMaxBindlessResources,
                      resourceCbDesc.Variables);
        return;
    }

    mShaderInputs.resources.reserve(resourceCbDesc.Variables);

    for(uint32_t variableIndex = 0; variableIndex < resourceCbDesc.Variables; ++variableIndex)
    {
        ID3D12ShaderReflectionVariable* variable = constantBufferReflection->GetVariableByIndex(variableIndex);

        D3D12_SHADER_VARIABLE_DESC variableDesc;
        variable->GetDesc(&variableDesc);

        ID3D12ShaderReflectionType* type = variable->GetType();

        D3D12_SHADER_TYPE_DESC variableTypeDesc;
        if(FAILED(type->GetDesc(&variableTypeDesc)))
        {
            spdlog::error("{}({}): Failed to get type information for variable '{}' in constant buffer '{}'.",
                          mParams.filepaths[(size_t)stage].generic_u8string(), stage, variableDesc.Name,
                          resourceCbDesc.Name);
            continue;
        }

        if(variableTypeDesc.Class != D3D_SVC_SCALAR && variableTypeDesc.Type != D3D_SVT_UINT)
        {
            spdlog::error("{}({}): '{}' in cbuffer '{}' must be a uint.",
                          mParams.filepaths[(size_t)stage].generic_u8string(), stage, variableDesc.Name,
                          resourceCbDesc.Name);
            continue;
        }

        mShaderInputs.resources.push_back(std::move(ParseBindlessResourceName(variableDesc.Name)));
    }
}
} // namespace scrap::d3d12