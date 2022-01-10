#pragma once

#include "d3d12/D3D12AccelerationStructureCommon.h"
#include "d3d12/D3D12BLAccelerationStructure.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12CommandList.h"

#include <memory>

#include <EASTL/vector.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x3.hpp>
#include <tl/expected.hpp>

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

class TLAccelerationStructure;

class TlasInstanceAllocation
{
public:
    TlasInstanceAllocation() = default;
    TlasInstanceAllocation(const TlasInstanceAllocation&) = delete;
    TlasInstanceAllocation(TlasInstanceAllocation&& other)
        : mAccelerationStructure(other.mAccelerationStructure)
        , mId(other.mId)
    {
        other.mAccelerationStructure = nullptr;
    }

    TlasInstanceAllocation(TLAccelerationStructure& accelerationStructure, size_t id)
        : mAccelerationStructure(&accelerationStructure)
        , mId(id)
    {}

    TlasInstanceAllocation& operator=(const TlasInstanceAllocation&) = delete;
    TlasInstanceAllocation& operator=(TlasInstanceAllocation&& other);

    ~TlasInstanceAllocation();

    void updateTransform(const glm::mat4x3& transform);

    bool isValid() const { return mAccelerationStructure != nullptr; }

private:
    TLAccelerationStructure* mAccelerationStructure = nullptr;
    size_t mId = std::numeric_limits<size_t>::max();
};

enum class TlasError
{
    InsufficientSpace,
};

struct TLAccelerationStructureInstanceParams
{
    std::shared_ptr<BLAccelerationStructure> accelerationStructure;
    glm::mat4x3 transform = glm::identity<glm::mat4x3>();
    TlasInstanceFlags flags = TlasInstanceFlags::None;
    uint32_t instanceId : 24 = 0xffffff;
    uint32_t instanceMask : 8 = 0xff;
    uint32_t instanceContributionToHitGroupIndex : 24 = 0;
    uint32_t padding : 8 = 0xff;
};

class TLAccelerationStructure
{
public:
    TLAccelerationStructure(const TLAccelerationStructureParams& params);

    const d3d12::Buffer& getAccelerationStructureBuffer() const { return *mAccelerationStructureGpuBuffer; }

    tl::expected<TlasInstanceAllocation, TlasError> addInstance(const TLAccelerationStructureInstanceParams& params);
    void removeInstanceById(size_t id);
    void updateInstanceTransformById(size_t id, const glm::mat4x3& transform);

    bool build(const GraphicsCommandList& commandList);

    void markAsUsed(const GraphicsCommandList& commandList);

    bool isReady() const;

    size_t getCapacity() const { return mParams.initialReservedObjects; }

private:
    bool doesInstanceDescsNeedResize(uint32_t newCapacity);
    void resizeInstanceDescsBuffer(uint32_t capacity);

    struct InternalInstance
    {
        size_t id;
        std::shared_ptr<BLAccelerationStructure> blas;
    };
    eastl::vector<InternalInstance> mInstances;
    eastl::vector<D3D12_RAYTRACING_INSTANCE_DESC> mInstanceDescs;
    size_t mNextId = 0;
    bool mIsDirty = false;

    TLAccelerationStructureParams mParams;

    std::unique_ptr<Buffer> mAccelerationStructureGpuBuffer;
    std::unique_ptr<Buffer> mScratchGpuBuffer;
    std::unique_ptr<Buffer> mInstanceDescsGpuBuffer;
    AccelerationStructureState mState = AccelerationStructureState::Invalid;
};
} // namespace scrap::d3d12