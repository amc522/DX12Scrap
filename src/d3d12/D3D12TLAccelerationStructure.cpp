#include "D3D12TLAccelerationStructure.h"

#include "d3d12/D3D12Context.h"

#include <d3dx12.h>
#include <fmt/format.h>

namespace scrap::d3d12
{
TLAccelerationStructure::TLAccelerationStructure(const TLAccelerationStructureParams& params): mParams(params)
{
    std::string instanceDescsName = fmt::format("{} (InstanceDescs)", params.name);

    Buffer::StructuredParams instanceDescsParams;
    instanceDescsParams.accessFlags = params.instanceDescs.accessFlags | ResourceAccessFlags::CpuWrite;
    instanceDescsParams.elementByteSize = AlignInteger((uint32_t)sizeof(D3D12_RAYTRACING_INSTANCE_DESC),
                                                       uint32_t(D3D12_RAYTRACING_INSTANCE_DESCS_BYTE_ALIGNMENT));
    instanceDescsParams.flags = params.instanceDescs.bufferFlags;
    instanceDescsParams.name = instanceDescsName;
    instanceDescsParams.numElements = (uint32_t)params.initialReservedObjects;

    mInstanceDescs.init(instanceDescsParams);

    mInstanceDescsWriteGuard =
        GpuBufferWriteGuard<Buffer>(mInstanceDescs, DeviceContext::instance().getCopyContext().getCommandList());
}

void TLAccelerationStructure::setInstance(const InstanceParams& params, size_t index)
{
    if(index >= mParams.initialReservedObjects) { return; }

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

    D3D12_RAYTRACING_INSTANCE_DESC& desc =
        mInstanceDescsWriteGuard.getWriteBufferAs<D3D12_RAYTRACING_INSTANCE_DESC>()[index];
    desc.AccelerationStructure = params.accelerationStructure->getBuffer().getResource()->GetGPUVirtualAddress();
    desc.Flags = flags;
    desc.InstanceContributionToHitGroupIndex = params.instanceContributionToHitGroupIndex;
    desc.InstanceID = params.instanceId;
    desc.InstanceMask = params.instanceMask;
    std::memcpy(desc.Transform, glm::value_ptr(transposedTransform), sizeof(desc.Transform));

    mInstanceAccelerationStructures.push_back(params.accelerationStructure);
}

bool TLAccelerationStructure::build(const GraphicsCommandList& commandList)
{
    mInstanceDescsWriteGuard = GpuBufferWriteGuard<Buffer>();

    auto device = d3d12::DeviceContext::instance().getDevice5();

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.Flags = TranslateAccelerationStructureBuildFlags(mParams.flags, mParams.buildOption);
    inputs.NumDescs = (uint32_t)mParams.initialReservedObjects;
    inputs.InstanceDescs = mInstanceDescs.getResource()->GetGPUVirtualAddress();
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

    if(prebuildInfo.ResultDataMaxSizeInBytes == 0) { return false; }

    {
        std::string scratchBufferName = fmt::format("{} (Scratch Buffer)", mParams.name);

        Buffer::SimpleParams params;
        params.accessFlags = ResourceAccessFlags::None;
        params.byteSize = prebuildInfo.ScratchDataSizeInBytes;
        params.flags = BufferFlags::UavEnabled;
        params.name = scratchBufferName;

        mScratchBuffer.init(params);
    }

    {
        Buffer::SimpleParams accelerationStructureParams;
        accelerationStructureParams.accessFlags = ResourceAccessFlags::None;
        accelerationStructureParams.byteSize = prebuildInfo.ResultDataMaxSizeInBytes;
        accelerationStructureParams.flags = BufferFlags::AccelerationStructure;
        accelerationStructureParams.initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        accelerationStructureParams.name = mParams.name;

        mAccelerationStructure.init(accelerationStructureParams);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
    desc.ScratchAccelerationStructureData = mScratchBuffer.getResource()->GetGPUVirtualAddress();
    desc.DestAccelerationStructureData = mAccelerationStructure.getResource()->GetGPUVirtualAddress();
    desc.Inputs = inputs;

    std::vector<D3D12_RESOURCE_BARRIER> blasTransitions;
    blasTransitions.reserve(mInstanceAccelerationStructures.size());

    for(size_t i = 0; i < mInstanceAccelerationStructures.size(); ++i)
    {
        blasTransitions.push_back(
            CD3DX12_RESOURCE_BARRIER::UAV(mInstanceAccelerationStructures[i]->getBuffer().getResource()));
    }

    commandList.get()->ResourceBarrier((uint32_t)blasTransitions.size(), blasTransitions.data());

    commandList.get4()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

    mScratchBuffer.markAsUsed(commandList.get());
    mInstanceDescs.markAsUsed(commandList.get());

    return true;
}

void TLAccelerationStructure::markAsUsed(const GraphicsCommandList& commandList)
{
    mAccelerationStructure.markAsUsed(commandList.get());

    for(std::shared_ptr<BLAccelerationStructure>& accelerationStructure : mInstanceAccelerationStructures)
    {
        accelerationStructure->markAsUsed(commandList);
    }
}
} // namespace scrap::d3d12