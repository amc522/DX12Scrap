#pragma once

#include "d3d12/D3D12AccelerationStructureCommon.h"
#include "d3d12/D3D12BLAccelerationStructure.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12CommandList.h"

#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x3.hpp>

namespace scrap::d3d12
{
struct TLAccelerationStructureParams : public AccelerationStructureParams
{
    struct
    {
        ResourceAccessFlags accessFlags = ResourceAccessFlags::None;
        BufferFlags bufferFlags = BufferFlags::None;
    } instanceDescs;
};

class TLAccelerationStructure
{
public:
    struct InstanceParams
    {
        std::shared_ptr<BLAccelerationStructure> accelerationStructure;
        glm::mat4x3 transform = glm::identity<glm::mat4x3>();
        uint32_t instanceId : 24;
        uint32_t instanceMask : 8;
        uint32_t instanceContributionToHitGroupIndex : 24;
        uint32_t flags : 8;
    };

    TLAccelerationStructure(const TLAccelerationStructureParams& params);

    const d3d12::Buffer& getAccelerationStructureBuffer() const { return mAccelerationStructure; }

    void setInstance(const InstanceParams& params, size_t index);

    bool build(const GraphicsCommandList& commandList);

    void markAsUsed(const GraphicsCommandList& commandList);

private:
    TLAccelerationStructureParams mParams;
    std::vector<std::shared_ptr<BLAccelerationStructure>> mInstanceAccelerationStructures;

    Buffer mAccelerationStructure;
    Buffer mScratchBuffer;
    Buffer mInstanceDescs;
    GpuBufferWriteGuard<Buffer> mInstanceDescsWriteGuard;
    AccelerationStructureState mState = AccelerationStructureState::Invalid;
};
} // namespace scrap::d3d12