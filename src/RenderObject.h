#pragma once

#include "SharedString.h"
#include "d3d12/D3D12ShaderTable.h"
#include "d3d12/D3D12TLAccelerationStructure.h"
#include "d3d12/D3D12Texture.h"

#include <memory>

#include <EASTL/vector_map.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x3.hpp>

namespace scrap
{
class GpuMesh;

namespace d3d12
{
class GraphicsPipelineState;
class RaytracingPipelineState;
} // namespace d3d12

class Material
{
public:
    eastl::vector_map<SharedString, std::shared_ptr<d3d12::Texture>> textures;
    std::shared_ptr<d3d12::GraphicsPipelineState> mRasterPipelineState;
    std::shared_ptr<d3d12::RaytracingPipelineState> mRaytracingPipelineState;
    d3d12::ShaderTableAllocation mShaderTableAllocation;
};

class RenderObjectId
{
public:
    explicit RenderObjectId(uint32_t id): mId(id) {}

    uint32_t value() const { return mId; }

    auto operator<=>(const RenderObjectId& other) const = default;

private:
    uint32_t mId;
};

class RenderObject
{
public:
    RenderObjectId mId;
    glm::mat4x3 mTransform = glm::identity<glm::mat4x3>();
    std::shared_ptr<GpuMesh> mGpuMesh;
    Material mMaterial;
    d3d12::TlasInstanceAllocation mInstanceAllocation;
    SharedString name;
};
} // namespace scrap