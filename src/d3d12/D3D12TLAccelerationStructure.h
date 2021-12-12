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

enum class TlasInstanceFlags : uint8_t
{
    None = 0,
    TriangleCullDisable = 1 << 0,
    TriangleFrontCcw = 1 << 1,
    ForceOpaque = 1 << 2,
    ForceNonOpaque = 1 << 3,
};
DEFINE_ENUM_BITWISE_OPERATORS(TlasInstanceFlags)

class TLAccelerationStructure
{
public:
    struct InstanceParams
    {
        std::shared_ptr<BLAccelerationStructure> accelerationStructure;
        glm::mat4x3 transform = glm::identity<glm::mat4x3>();
        TlasInstanceFlags flags;
        uint32_t instanceId : 24;
        uint32_t instanceMask : 8;
        uint32_t instanceContributionToHitGroupIndex : 24;
        uint32_t padding : 8;
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