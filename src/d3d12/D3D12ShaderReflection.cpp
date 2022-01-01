#include "D3D12ShaderReflection.h"

#include "StringUtils.h"
#include "d3d12/D3D12Config.h"

#include <charconv>

#include <d3d12.h>
#include <d3d12shader.h>

namespace scrap::d3d12::ShaderReflection
{
template<class Pred>
[[nodiscard]] std::optional<ShaderReflectionErrorInfo>
ParseBindlessConstantBufferVariable(const D3D12_SHADER_BUFFER_DESC& constantBufferDesc,
                                    ID3D12ShaderReflectionType* shaderVariableType,
                                    std::string_view name,
                                    uint32_t byteOffset,
                                    Pred pred)
{
    D3D12_SHADER_TYPE_DESC variableTypeDesc;
    shaderVariableType->GetDesc(&variableTypeDesc);

    if(variableTypeDesc.Class == D3D_SVC_SCALAR && variableTypeDesc.Type == D3D_SVT_UINT)
    {
        pred(name, byteOffset / sizeof(uint32_t));
    }
    else if(variableTypeDesc.Class == D3D_SVC_STRUCT)
    {
        std::optional errorInfo =
            EnumerateBindlessConstantBufferVariables(constantBufferDesc, shaderVariableType, byteOffset, pred);

        if(errorInfo) { return errorInfo; }
    }
    else
    {
        return ShaderReflectionErrorInfo{.error = ShaderReflectionError::InvalidMemberType,
                                         .variableType = variableTypeDesc.Type,
                                         .variableTypeRows = variableTypeDesc.Rows,
                                         .variableTypeColumns = variableTypeDesc.Columns,
                                         .variableElements = variableTypeDesc.Elements};
    }

    return std::nullopt;
}

template<class Pred>
[[nodiscard]] std::optional<ShaderReflectionErrorInfo>
EnumerateBindlessConstantBufferVariables(const D3D12_SHADER_BUFFER_DESC& constantBufferDesc,
                                         ID3D12ShaderReflectionType* structType,
                                         uint32_t byteOffset,
                                         Pred pred)
{
    D3D12_SHADER_TYPE_DESC structTypeDesc;
    structType->GetDesc(&structTypeDesc);

    for(uint32_t memberIndex = 0; memberIndex < structTypeDesc.Members; ++memberIndex)
    {
        ID3D12ShaderReflectionType* memberType = structType->GetMemberTypeByIndex(memberIndex);

        D3D12_SHADER_TYPE_DESC memberTypeDesc;
        memberType->GetDesc(&memberTypeDesc);

        const std::string_view name = structType->GetMemberTypeName(memberIndex);

        std::optional errorInfo = ParseBindlessConstantBufferVariable(constantBufferDesc, memberType, name,
                                                                      byteOffset + memberTypeDesc.Offset, pred);

        if(errorInfo)
        {
            errorInfo->memberName = name;
            return errorInfo;
        }
    }

    return std::nullopt;
}

template<class Pred>
[[nodiscard]] std::optional<ShaderReflectionErrorInfo>
EnumerateBindlessConstantBufferVariables(ID3D12ShaderReflectionConstantBuffer* shaderConstantBuffer, Pred pred)
{
    D3D12_SHADER_BUFFER_DESC constantBufferDesc;
    shaderConstantBuffer->GetDesc(&constantBufferDesc);

    for(uint32_t variableIndex = 0; variableIndex < constantBufferDesc.Variables; ++variableIndex)
    {
        ID3D12ShaderReflectionVariable* shaderVariable = shaderConstantBuffer->GetVariableByIndex(variableIndex);

        D3D12_SHADER_VARIABLE_DESC shaderVariableDesc;
        shaderVariable->GetDesc(&shaderVariableDesc);

        std::optional errorInfo =
            ParseBindlessConstantBufferVariable(constantBufferDesc, shaderVariable->GetType(), shaderVariableDesc.Name,
                                                shaderVariableDesc.StartOffset, pred);
        if(errorInfo)
        {
            errorInfo->constantBufferName = constantBufferDesc.Name;
            return errorInfo;
        }
    }

    return std::nullopt;
}

[[nodiscard]] ShaderResource ParseBindlessResourceName(std::string_view name,
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
    size_t nextSeparatorPos = name.find("$");
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
    nextSeparatorPos = name.find("$");
    if(nextSeparatorPos == std::string_view::npos) { return {}; }

    std::string_view fullReturnTypeStr = name.substr(0, nextSeparatorPos);

    // check for _t types
    auto underscoreTPos = fullReturnTypeStr.find("_t");

    std::string_view returnTypeStr;
    std::string_view componentCountStr;

    if(underscoreTPos != std::string_view::npos)
    {
        returnTypeStr = fullReturnTypeStr.substr(0, underscoreTPos + 2);
        componentCountStr = fullReturnTypeStr.substr(underscoreTPos + 2);
    }
    else
    {
        const size_t componentCountPos = name.find_first_of("1234");

        if(componentCountPos != std::string_view::npos)
        {
            returnTypeStr = fullReturnTypeStr.substr(0, componentCountPos);
            componentCountStr = fullReturnTypeStr.substr(componentCountPos);
        }
        else
        {
            returnTypeStr = fullReturnTypeStr;
        }
    }

    if(!componentCountStr.empty())
    {
        std::from_chars(componentCountStr.data(), componentCountStr.data() + componentCountStr.size(),
                        resource.returnTypeComponentCount);
    }

    if(returnTypeStr == "unorm") { resource.returnType = ShaderResourceReturnType::Unorm; }
    else if(returnTypeStr == "snorm")
    {
        resource.returnType = ShaderResourceReturnType::Snorm;
    }
    else if(returnTypeStr == "int16_t")
    {
        resource.returnType = ShaderResourceReturnType::Int16;
    }
    else if(returnTypeStr == "uint16_t")
    {
        resource.returnType = ShaderResourceReturnType::UInt16;
    }
    else if(returnTypeStr == "int")
    {
        resource.returnType = ShaderResourceReturnType::Int;
    }
    else if(returnTypeStr == "uint")
    {
        resource.returnType = ShaderResourceReturnType::UInt;
    }
    else if(returnTypeStr == "int64_t")
    {
        resource.returnType = ShaderResourceReturnType::Int64;
    }
    else if(returnTypeStr == "uint64_t")
    {
        resource.returnType = ShaderResourceReturnType::UInt64;
    }
    else if(returnTypeStr == "half" || returnTypeStr == "float16_t")
    {
        resource.returnType = ShaderResourceReturnType::Half;
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

[[nodiscard]] ShaderVertexElement ParseBindlessVertexBufferName(std::string_view name,
                                                                std::string_view prefix = "g",
                                                                std::string_view suffix = "DescriptorIndex")
{
    ShaderVertexElement vertexElement{ParseBindlessResourceName(name, prefix, suffix)};

    if(vertexElement.name.empty()) { return {}; }

    name = vertexElement.name;

    if(StartsWith(name, std::string_view("Vertex"))) { name.remove_prefix(6); }

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

bool IsVertexIndicesRegister(const D3D12_SHADER_INPUT_BIND_DESC& desc)
{
    return desc.Type == D3D_SIT_CBUFFER && desc.BindPoint == reservedShaderRegister::kVertexCB.shaderRegister &&
           desc.Space == reservedShaderRegister::kVertexCB.registerSpace;
}

bool IsResourceIndicesRegister(const D3D12_SHADER_INPUT_BIND_DESC& desc)
{
    return desc.Type == D3D_SIT_CBUFFER && desc.BindPoint == reservedShaderRegister::kResourceCB.shaderRegister &&
           desc.Space == reservedShaderRegister::kResourceCB.registerSpace;
}

tl::expected<VertexInputsCbInfo, ShaderReflectionErrorInfo>
CollectVertexInputs(ID3D12ShaderReflection* reflection, const D3D12_SHADER_INPUT_BIND_DESC& shaderInputBindDesc)
{
    if(shaderInputBindDesc.Type != D3D_SIT_CBUFFER)
    {
        return tl::make_unexpected(
            ShaderReflectionErrorInfo{.error = ShaderReflectionError::ResourceInReservedRegisterSpace});
    }

    ID3D12ShaderReflectionConstantBuffer* constantBufferReflection =
        reflection->GetConstantBufferByIndex(shaderInputBindDesc.uID);

    if(constantBufferReflection == nullptr)
    {
        return tl::make_unexpected(ShaderReflectionErrorInfo{.error = ShaderReflectionError::GetConstantBufferFailure,
                                                             .constantBufferName = shaderInputBindDesc.Name});
    }

    return CollectVertexInputs(constantBufferReflection);
}

tl::expected<VertexInputsCbInfo, ShaderReflectionErrorInfo>
CollectVertexInputs(ID3D12ShaderReflectionConstantBuffer* constantBufferReflection)
{
    D3D12_SHADER_BUFFER_DESC vertexCbDesc;
    if(FAILED(constantBufferReflection->GetDesc(&vertexCbDesc)))
    {
        return tl::make_unexpected(
            ShaderReflectionErrorInfo{.error = ShaderReflectionError::GetConstantBufferDescFailure});
    }

    size_t vertexBufferCount = 0;

    // First enumerate the variables and count them up
    std::optional errorInfo = EnumerateBindlessConstantBufferVariables(
        constantBufferReflection, [&vertexBufferCount](std::string_view, uint32_t) { ++vertexBufferCount; });

    if(errorInfo)
    {
        errorInfo->constantBufferName = vertexCbDesc.Name;
        return tl::make_unexpected(std::move(errorInfo.value()));
    }

    if(vertexBufferCount > d3d12::kMaxBindlessVertexBuffers)
    {
        return tl::make_unexpected(ShaderReflectionErrorInfo{.error = ShaderReflectionError::ExceededMaxResourceCount,
                                                             .constantBufferName = vertexCbDesc.Name,
                                                             .resourceOverflowCount = vertexBufferCount});
    }

    // Now actually parse each variable
    VertexInputsCbInfo info;
    info.vertexElements.reserve(vertexBufferCount);

    errorInfo =
        EnumerateBindlessConstantBufferVariables(constantBufferReflection, [&](std::string_view name, uint32_t index) {
            ShaderVertexElement vertexElement = ParseBindlessVertexBufferName(name);
            vertexElement.index = index;

            info.vertexElements.push_back(std::move(vertexElement));
        });

    if(errorInfo)
    {
        errorInfo->constantBufferName = vertexCbDesc.Name;
        return tl::make_unexpected(std::move(errorInfo.value()));
    }

    return info;
}

tl::expected<ResourcesCbInfo, ShaderReflectionErrorInfo>
CollectResourceInputs(ID3D12ShaderReflection* reflection, const D3D12_SHADER_INPUT_BIND_DESC& shaderInputBindDesc)
{
    if(shaderInputBindDesc.Type != D3D_SIT_CBUFFER)
    {
        return tl::make_unexpected(
            ShaderReflectionErrorInfo{.error = ShaderReflectionError::ResourceInReservedRegisterSpace});
    }

    ID3D12ShaderReflectionConstantBuffer* constantBufferReflection =
        reflection->GetConstantBufferByIndex(shaderInputBindDesc.uID);

    if(constantBufferReflection == nullptr)
    {
        return tl::make_unexpected(ShaderReflectionErrorInfo{.error = ShaderReflectionError::GetConstantBufferFailure,
                                                             .constantBufferName = shaderInputBindDesc.Name});
    }

    return CollectResourceInputs(constantBufferReflection);
}

tl::expected<ResourcesCbInfo, ShaderReflectionErrorInfo>
CollectResourceInputs(ID3D12ShaderReflectionConstantBuffer* constantBufferReflection)
{
    D3D12_SHADER_BUFFER_DESC resourceCbDesc;
    if(FAILED(constantBufferReflection->GetDesc(&resourceCbDesc)))
    {
        return tl::make_unexpected(
            ShaderReflectionErrorInfo{.error = ShaderReflectionError::GetConstantBufferDescFailure});
    }

    // First enumerate the variables and count them up
    size_t resourceCount = 0;
    std::optional errorInfo = EnumerateBindlessConstantBufferVariables(
        constantBufferReflection, [&resourceCount](std::string_view, uint32_t) { ++resourceCount; });

    if(errorInfo)
    {
        errorInfo->constantBufferName = resourceCbDesc.Name;
        return tl::make_unexpected(std::move(errorInfo.value()));
    }

    if(resourceCount > d3d12::kMaxBindlessResources)
    {
        return tl::make_unexpected(ShaderReflectionErrorInfo{.error = ShaderReflectionError::ExceededMaxResourceCount,
                                                             .constantBufferName = resourceCbDesc.Name,
                                                             .resourceOverflowCount = resourceCount});
    }

    // Now actually parse each variable
    ResourcesCbInfo info;
    info.resources.reserve(resourceCount);

    errorInfo =
        EnumerateBindlessConstantBufferVariables(constantBufferReflection, [&](std::string_view name, uint32_t index) {
            ShaderResource resource = ParseBindlessResourceName(name);
            resource.index = index;

            info.resources.push_back(std::move(resource));
        });

    if(errorInfo)
    {
        errorInfo->constantBufferName = resourceCbDesc.Name;
        return tl::make_unexpected(std::move(errorInfo.value()));
    }

    return info;
}
} // namespace scrap::d3d12::ShaderReflection