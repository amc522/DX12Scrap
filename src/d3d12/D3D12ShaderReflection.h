#pragma once

#include "RenderDefs.h"

#include <optional>

#include <d3dcommon.h>
#include <tl/expected.hpp>

struct ID3D12ShaderReflection;
struct ID3D12ShaderReflectionConstantBuffer;

struct _D3D12_SHADER_BUFFER_DESC;
using D3D12_SHADER_BUFFER_DESC = _D3D12_SHADER_BUFFER_DESC;

struct _D3D12_SHADER_INPUT_BIND_DESC;
using D3D12_SHADER_INPUT_BIND_DESC = _D3D12_SHADER_INPUT_BIND_DESC;

enum _D3D_SHADER_INPUT_TYPE;
using D3D_SHADER_INPUT_TYPE = _D3D_SHADER_INPUT_TYPE;

namespace scrap::d3d12
{
enum class ShaderReflectionError
{
    ResourceInReservedRegisterSpace,
    GetConstantBufferFailure,
    GetConstantBufferDescFailure,
    InvalidMemberType,
    ExceededMaxResourceCount,
};

struct ShaderReflectionErrorInfo
{
    ShaderReflectionError error;
    std::string constantBufferName;
    std::string memberName;
    D3D_SHADER_VARIABLE_TYPE variableType;
    uint32_t variableTypeRows = 0;
    uint32_t variableTypeColumns = 0;
    uint32_t variableElements = 0;
    size_t resourceOverflowCount = 0;
};

struct VertexInputsCbInfo
{
    std::vector<ShaderVertexElement> vertexElements;
};

struct ResourcesCbInfo
{
    std::vector<ShaderResource> resources;
};

namespace ShaderReflection
{
[[nodiscard]] bool IsValidGlobalResourceType(D3D_SHADER_INPUT_TYPE type);

[[nodiscard]] bool IsVertexIndicesRegister(const D3D12_SHADER_INPUT_BIND_DESC& desc);
[[nodiscard]] bool IsVertexIndicesRegister(const D3D12_SHADER_BUFFER_DESC& desc);

[[nodiscard]] bool IsResourceIndicesRegister(const D3D12_SHADER_INPUT_BIND_DESC& desc);
[[nodiscard]] bool IsResourceIndicesRegister(const D3D12_SHADER_BUFFER_DESC& desc);

[[nodiscard]] tl::expected<VertexInputsCbInfo, ShaderReflectionErrorInfo>
CollectVertexInputs(ID3D12ShaderReflection* reflection, const D3D12_SHADER_INPUT_BIND_DESC& shaderInputBindDesc);

[[nodiscard]] tl::expected<VertexInputsCbInfo, ShaderReflectionErrorInfo>
CollectVertexInputs(ID3D12ShaderReflectionConstantBuffer* constantBufferReflection);

[[nodiscard]] tl::expected<ResourcesCbInfo, ShaderReflectionErrorInfo>
CollectResourceInputs(ID3D12ShaderReflection* reflection, const D3D12_SHADER_INPUT_BIND_DESC& shaderInputBindDesc);

[[nodiscard]] tl::expected<ResourcesCbInfo, ShaderReflectionErrorInfo>
CollectResourceInputs(ID3D12ShaderReflectionConstantBuffer* constantBufferReflection);
} // namespace ShaderReflection
} // namespace scrap::d3d12