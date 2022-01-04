#pragma once

#include "SharedString.h"
#include "d3d12/D3D12ShaderTable.h"
#include "d3d12/D3D12TLAccelerationStructure.h"
#include "d3d12/D3D12Texture.h"

#include <memory>

#include <EASTL/vector_map.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
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

struct Transform
{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation = glm::identity<glm::quat>();
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 getMatrix4x4() const
    {
        auto transform = glm::identity<glm::mat4x4>();
        transform = glm::translate(transform, position);
        transform = transform * glm::mat4_cast(rotation);
        transform = glm::scale(transform, scale);

        return transform;
    }
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
    Transform mTransform;
    std::shared_ptr<GpuMesh> mGpuMesh;
    Material mMaterial;
    d3d12::TlasInstanceAllocation mInstanceAllocation;
    SharedString name;
};
} // namespace scrap