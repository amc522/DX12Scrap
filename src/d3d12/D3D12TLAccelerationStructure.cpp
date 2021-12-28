#include "D3D12TLAccelerationStructure.h"

#include "SpanUtility.h"
#include "d3d12/D3D12Context.h"

#include <d3dx12.h>
#include <fmt/format.h>

namespace scrap::d3d12
{
TLAccelerationStructure::TLAccelerationStructure(const TLAccelerationStructureParams& params): mParams(params)
{
    resizeInstanceDescsBuffer((uint32_t)params.initialReservedObjects);
}

tl::expected<TlasInstanceAllocation, TlasError>
TLAccelerationStructure::addInstance(const TLAccelerationStructureInstanceParams& params)
{
    const glm::mat3x4 transposedTransform = glm::transpose(params.transform);

    uint8_t flags = 0;

    if((params.flags & TlasInstanceFlags::TriangleCullDisable) == TlasInstanceFlags::TriangleCullDisable)
    {
        flags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE;
    }
    if((params.flags & TlasInstanceFlags::TriangleFrontCcw) == TlasInstanceFlags::TriangleFrontCcw)
    {
        flags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
    }
    if((params.flags & TlasInstanceFlags::ForceOpaque) == TlasInstanceFlags::ForceOpaque)
    {
        flags |= D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;
    }
    if((params.flags & TlasInstanceFlags::ForceNonOpaque) == TlasInstanceFlags::ForceNonOpaque)
    {
        flags |= D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_NON_OPAQUE;
    }

    InternalInstance& internalInstance = mInstances.emplace_back();
    internalInstance.blas = params.accelerationStructure;
    internalInstance.id = mNextId++;

    D3D12_RAYTRACING_INSTANCE_DESC& instanceDesc = mInstanceDescs.emplace_back();
    instanceDesc.AccelerationStructure =
        params.accelerationStructure->getBuffer().getResource()->GetGPUVirtualAddress();
    instanceDesc.Flags = flags;
    instanceDesc.InstanceContributionToHitGroupIndex = params.instanceContributionToHitGroupIndex;
    instanceDesc.InstanceID = params.instanceId;
    instanceDesc.InstanceMask = params.instanceMask;
    std::memcpy(instanceDesc.Transform, glm::value_ptr(transposedTransform), sizeof(instanceDesc.Transform));

    mIsDirty = true;

    return TlasInstanceAllocation(*this, internalInstance.id);
}

void TLAccelerationStructure::removeInstanceById(size_t id)
{
    auto itr = std::find_if(mInstances.begin(), mInstances.end(),
                            [id](const InternalInstance& params) { return params.id == id; });

    if(itr == mInstances.end()) { return; }

    mInstances.erase_unsorted(itr);
    mInstanceDescs.erase_unsorted(mInstanceDescs.begin() + std::distance(mInstances.begin(), itr));

    mIsDirty = true;
}

void TLAccelerationStructure::updateInstanceTransformById(size_t id, const glm::mat4x3& transform)
{
    auto itr = std::find_if(mInstances.begin(), mInstances.end(),
                            [id](const InternalInstance& params) { return params.id == id; });

    if(itr == mInstances.end()) { return; }

    const glm::mat3x4 transposedTransform = glm::transpose(transform);

    size_t instanceDescIndex = std::distance(mInstances.begin(), itr);
    std::memcpy(mInstanceDescs[instanceDescIndex].Transform, glm::value_ptr(transposedTransform),
                sizeof(D3D12_RAYTRACING_INSTANCE_DESC::Transform));

    mIsDirty = true;
}

bool TLAccelerationStructure::build(const GraphicsCommandList& commandList)
{
    auto device = d3d12::DeviceContext::instance().getDevice5();

    if(doesInstanceDescsNeedResize((uint32_t)mInstances.size()))
    {
        resizeInstanceDescsBuffer((uint32_t)mInstances.size());
    }
    else if(mIsDirty)
    {
        {
            GpuBufferWriteGuard<Buffer> writeGuard(*mInstanceDescsGpuBuffer, commandList.get());
            auto writeBuffer = writeGuard.getWriteBufferAs<D3D12_RAYTRACING_INSTANCE_DESC>();

            std::copy(mInstanceDescs.cbegin(), mInstanceDescs.cend(), writeBuffer.begin());
        }

        mInstanceDescsGpuBuffer->transition(commandList.get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.Flags = TranslateAccelerationStructureBuildFlags(mParams.flags, mParams.buildOption);
    inputs.NumDescs = (uint32_t)mInstances.size();
    inputs.InstanceDescs = mInstanceDescsGpuBuffer->getResource()->GetGPUVirtualAddress();
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

    if(prebuildInfo.ResultDataMaxSizeInBytes == 0) { return false; }

    if(mScratchGpuBuffer == nullptr || mScratchGpuBuffer->getByteSize() < prebuildInfo.ScratchDataSizeInBytes)
    {
        mScratchGpuBuffer = std::make_unique<Buffer>();
        std::string scratchBufferName = fmt::format("{} (Scratch Buffer)", mParams.name);

        BufferSimpleParams params;
        params.accessFlags = ResourceAccessFlags::None;
        params.byteSize = prebuildInfo.ScratchDataSizeInBytes;
        params.flags = BufferFlags::UavEnabled;
        params.name = scratchBufferName;

        mScratchGpuBuffer->init(params);
    }

    if(mAccelerationStructureGpuBuffer == nullptr ||
       mAccelerationStructureGpuBuffer->getByteSize() < prebuildInfo.ResultDataMaxSizeInBytes)
    {
        mAccelerationStructureGpuBuffer = std::make_unique<Buffer>();

        BufferSimpleParams accelerationStructureParams;
        accelerationStructureParams.accessFlags = ResourceAccessFlags::None;
        accelerationStructureParams.byteSize = prebuildInfo.ResultDataMaxSizeInBytes;
        accelerationStructureParams.flags = BufferFlags::AccelerationStructure;
        accelerationStructureParams.initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        accelerationStructureParams.name = mParams.name;

        mAccelerationStructureGpuBuffer->init(accelerationStructureParams);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
    desc.ScratchAccelerationStructureData = mScratchGpuBuffer->getResource()->GetGPUVirtualAddress();
    desc.DestAccelerationStructureData = mAccelerationStructureGpuBuffer->getResource()->GetGPUVirtualAddress();
    desc.Inputs = inputs;

    /*std::vector<D3D12_RESOURCE_BARRIER> blasTransitions;
    blasTransitions.reserve(mInstanceAccelerationStructures.size());

    for(size_t i = 0; i < mInstanceAccelerationStructures.size(); ++i)
    {
        blasTransitions.push_back(
            CD3DX12_RESOURCE_BARRIER::UAV(mInstanceAccelerationStructures[i]->getBuffer().getResource()));
    }*/

    // commandList.get()->ResourceBarrier((uint32_t)blasTransitions.size(), blasTransitions.data());

    commandList.get4()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

    mScratchGpuBuffer->markAsUsed(commandList.get());
    mInstanceDescsGpuBuffer->markAsUsed(commandList.get());
    mAccelerationStructureGpuBuffer->markAsUsed(commandList.get());

    return true;
}

void TLAccelerationStructure::markAsUsed(const GraphicsCommandList& commandList)
{
    mAccelerationStructureGpuBuffer->markAsUsed(commandList.get());

    for(InternalInstance& instance : mInstances)
    {
        instance.blas->markAsUsed(commandList);
    }
}

bool TLAccelerationStructure::isReady() const
{
    return mAccelerationStructureGpuBuffer != nullptr && mAccelerationStructureGpuBuffer->isReady();
}

bool TLAccelerationStructure::doesInstanceDescsNeedResize(uint32_t newCapacity)
{
    uint32_t requiredByteSize = AlignInteger((uint32_t)sizeof(D3D12_RAYTRACING_INSTANCE_DESC),
                                             uint32_t(D3D12_RAYTRACING_INSTANCE_DESCS_BYTE_ALIGNMENT)) *
                                newCapacity;

    return mInstanceDescsGpuBuffer == nullptr || mInstanceDescsGpuBuffer->getByteSize() < requiredByteSize;
}

void TLAccelerationStructure::resizeInstanceDescsBuffer(uint32_t capacity)
{
    mInstanceDescsGpuBuffer = std::make_unique<d3d12::Buffer>();

    std::string instanceDescsName = fmt::format("{} (InstanceDescs)", mParams.name);

    BufferStructuredParams instanceDescsParams;
    instanceDescsParams.accessFlags = mParams.instanceDescs.accessFlags | ResourceAccessFlags::CpuWrite;
    instanceDescsParams.elementByteSize = AlignInteger((uint32_t)sizeof(D3D12_RAYTRACING_INSTANCE_DESC),
                                                       uint32_t(D3D12_RAYTRACING_INSTANCE_DESCS_BYTE_ALIGNMENT));
    instanceDescsParams.flags = mParams.instanceDescs.bufferFlags | BufferFlags::NonPixelShaderResource;
    instanceDescsParams.name = instanceDescsName;
    instanceDescsParams.numElements = capacity;

    mInstanceDescsGpuBuffer->init(instanceDescsParams, ToByteSpan(mInstanceDescs));
}

TlasInstanceAllocation& TlasInstanceAllocation::operator=(TlasInstanceAllocation&& other)
{
    mAccelerationStructure = other.mAccelerationStructure;
    other.mAccelerationStructure = nullptr;

    mId = other.mId;
    other.mId = std::numeric_limits<size_t>::max();

    return *this;
}

TlasInstanceAllocation::~TlasInstanceAllocation()
{
    if(mAccelerationStructure == nullptr) { return; }

    mAccelerationStructure->removeInstanceById(mId);
}

void TlasInstanceAllocation::updateTransform(const glm::mat4x3& transform)
{
    mAccelerationStructure->updateInstanceTransformById(mId, transform);
}
} // namespace scrap::d3d12