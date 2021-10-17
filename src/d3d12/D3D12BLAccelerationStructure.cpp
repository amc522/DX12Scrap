#include "D3D12BLAccelerationStructure.h"

#include "d3d12/D3D12Context.h"

#include <d3dx12.h>

namespace scrap::d3d12
{
BLAccelerationStructure::BLAccelerationStructure(const BLAccelerationStructureParams& params): mParams(params)
{
    reserve(params.initialReservedObjects);
}

void BLAccelerationStructure::reserve(size_t count)
{
    mGeometryDescs.reserve(count);
    mGeometryBuffers.reserve(count * 2);
}

void BLAccelerationStructure::addMesh(const GeometryParams& geometryParams)
{
    D3D12_RAYTRACING_GEOMETRY_FLAGS flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

    if((RaytracingGeometryFlags::Transparent & geometryParams.flags) == RaytracingGeometryFlags::None)
    {
        flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    }
    if((RaytracingGeometryFlags::NoDuplicateAnyHit & geometryParams.flags) ==
       RaytracingGeometryFlags::NoDuplicateAnyHit)
    {
        flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
    }

    D3D12_RAYTRACING_GEOMETRY_DESC& desc = mGeometryDescs.emplace_back();
    desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    desc.Flags = flags;
    desc.Triangles.IndexBuffer = geometryParams.indexBuffer->getResource()->GetGPUVirtualAddress();
    desc.Triangles.IndexCount = (UINT)geometryParams.indexBuffer->getElementCount();
    desc.Triangles.IndexFormat = geometryParams.indexBuffer->getFormat();
    desc.Triangles.VertexBuffer.StartAddress = geometryParams.vertexBuffer->getResource()->GetGPUVirtualAddress();
    desc.Triangles.VertexBuffer.StrideInBytes = geometryParams.vertexBuffer->getElementByteSize();
    desc.Triangles.VertexCount = (UINT)geometryParams.vertexBuffer->getElementCount();
    desc.Triangles.VertexFormat = geometryParams.vertexBuffer->getFormat();
    desc.Triangles.Transform3x4 = 0; // nullptr

    mGeometryBuffers.push_back(geometryParams.indexBuffer);
    mGeometryBuffers.push_back(geometryParams.vertexBuffer);
}

void BLAccelerationStructure::addMesh(nonstd::span<const GeometryParams> geometries)
{
    mGeometryDescs.reserve(geometries.size() + mGeometryDescs.size());

    for(const GeometryParams& geometryParams : geometries)
    {
        addMesh(geometryParams);
    }
}

bool BLAccelerationStructure::build(const GraphicsCommandList& commandList)
{
    ID3D12Device5* device = DeviceContext::instance().getDevice5();

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.Flags = TranslateAccelerationStructureBuildFlags(mParams.flags, mParams.buildOption);
    inputs.NumDescs = (UINT)mGeometryDescs.size();
    inputs.pGeometryDescs = mGeometryDescs.data();

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

    if(prebuildInfo.ResultDataMaxSizeInBytes == 0) { return false; }

    {
        std::string scratchBufferName = fmt::format("{} (Scratch Buffer)", mParams.name);

        Buffer::SimpleParams scratchBufferParams;
        scratchBufferParams.accessFlags = ResourceAccessFlags::None;
        scratchBufferParams.byteSize = prebuildInfo.ScratchDataSizeInBytes;
        scratchBufferParams.flags = BufferFlags::UavEnabled;
        scratchBufferParams.name = scratchBufferName;

        mScratchBuffer.init(scratchBufferParams);
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

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
    buildDesc.Inputs = inputs;
    buildDesc.DestAccelerationStructureData = mAccelerationStructure.getResource()->GetGPUVirtualAddress();
    buildDesc.ScratchAccelerationStructureData = mScratchBuffer.getResource()->GetGPUVirtualAddress();

    commandList.get4()->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

    mScratchBuffer.markAsUsed(commandList.get());

    return true;
}

void BLAccelerationStructure::markAsUsed(const GraphicsCommandList& commandList)
{
    mAccelerationStructure.markAsUsed(commandList.get());

    for(const std::shared_ptr<Buffer>& geomBuffer : mGeometryBuffers)
    {
        geomBuffer->markAsUsed(commandList.get());
    }
}
} // namespace scrap::d3d12