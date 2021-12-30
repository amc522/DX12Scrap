#pragma once

#include "d3d12/D3D12AccelerationStructureCommon.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12CommandList.h"

#include <memory>
#include <vector>

#include <d3d12.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x3.hpp>

namespace scrap::d3d12
{
struct BLAccelerationStructureParams : public AccelerationStructureParams
{};

struct BLAccelerationStructureGeometryParams
{
    std::shared_ptr<Buffer> indexBuffer;
    std::shared_ptr<Buffer> vertexBuffer;
    RaytracingGeometryFlags flags = RaytracingGeometryFlags::None;
};

class BLAccelerationStructure
{
public:
    BLAccelerationStructure(const BLAccelerationStructureParams& params);

    const Buffer& getBuffer() const { return mAccelerationStructure; }

    void reserve(size_t count);

    void addMesh(const BLAccelerationStructureGeometryParams& geometryParams);
    void addMesh(std::span<const BLAccelerationStructureGeometryParams> geometries);

    bool build(const GraphicsCommandList& commandList);

    void markAsUsed(const GraphicsCommandList& commandList);

private:
    BLAccelerationStructureParams mParams;
    std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> mGeometryDescs;
    std::vector<std::shared_ptr<Buffer>> mGeometryBuffers;

    Buffer mAccelerationStructure;
    Buffer mScratchBuffer;
    AccelerationStructureState mState = AccelerationStructureState::Invalid;
};
} // namespace scrap::d3d12