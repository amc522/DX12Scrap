#pragma once

#include <span>

#include <glm/ext/vector_uint3_sized.hpp>
#include <glm/vec3.hpp>

namespace scrap::d3d12
{
class Buffer;
class GraphicsCommandList;
class RaytracingPipelineState;
class ShaderTable;
class Texture;
class TLAccelerationStructure;

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

bool dispatchRays(d3d12::GraphicsCommandList& commandList, const DispatchRaysParams& params);
} // namespace scrap::d3d12