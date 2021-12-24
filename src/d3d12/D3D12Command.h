#pragma once

#include "RenderDefs.h"

#include <optional>
#include <span>

#include <glm/ext/vector_uint3_sized.hpp>
#include <glm/vec3.hpp>

namespace scrap::d3d12
{
class Buffer;
class GpuMesh;
class GraphicsCommandList;
class GraphicsPipelineState;
class RaytracingPipelineState;
class ShaderTable;
class Texture;
class TLAccelerationStructure;

struct VertexBuffer;

enum class CommandError
{
    ResourcesNotReady,
};

struct EngineConstantBuffers
{
    d3d12::Buffer* frame = nullptr;
};

struct DispatchRaysParams
{
    d3d12::RaytracingPipelineState* pipelineState = nullptr;
    std::span<d3d12::TLAccelerationStructure*> accelerationStructures;
    d3d12::ShaderTable* shaderTable = nullptr;
    EngineConstantBuffers constantBuffers;
    std::span<d3d12::Texture*> rwTextures;
    glm::u32vec3 dimensions{0, 0, 0};
};

struct DrawIndexedParams
{
    d3d12::Buffer* indexBuffer = nullptr;
    d3d12::GraphicsPipelineState* pipelineState = nullptr;
    std::span<const VertexBuffer> vertexBuffers;
    PrimitiveTopology primitiveTopology = PrimitiveTopology::Undefined;
    uint32_t indexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t indexOffset = 0;
    uint32_t vertexOffset = 0;
    uint32_t instanceOffset = 0;
};

std::optional<CommandError> drawIndexed(d3d12::GraphicsCommandList& commandList, const DrawIndexedParams& params);

std::optional<CommandError> dispatchRays(d3d12::GraphicsCommandList& commandList, const DispatchRaysParams& params);
} // namespace scrap::d3d12