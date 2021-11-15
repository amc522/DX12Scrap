#include "d3d12/D3D12ShaderTable.h"

#include "EnumIterator.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12RaytracingPipelineState.h"

#include <d3d12.h>

namespace scrap::d3d12
{
void ShaderTable::init(const ShaderTableParams& params)
{
    mParams[(size_t)RaytracingPipelineStage::RayGen] = params.raygen;
    mParams[(size_t)RaytracingPipelineStage::HitGroup] = params.raygen;
    mParams[(size_t)RaytracingPipelineStage::Miss] = params.raygen;

    std::array<std::string, (size_t)RaytracingPipelineStage::Count> tableNames{
        fmt::format("{} (Raygen Table)", params.name),
        fmt::format("{} (Hit Group Table)", params.name),
        fmt::format("{} (Miss Table)", params.name),
    };

    Buffer::SimpleParams bufferParams;
    bufferParams.accessFlags = ResourceAccessFlags::CpuWrite;
    bufferParams.flags = BufferFlags::None;
    bufferParams.initialResourceState = D3D12_RESOURCE_STATE_COMMON;

    for(RaytracingPipelineStage stage : Enumerator<RaytracingPipelineStage>())
    {
        ShaderTableStageParams& params = mParams[(size_t)stage];
        params.entryByteStride = AlignInteger(detail::kShaderIdentifierSize + params.entryByteStride,
                                              size_t(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT));

        bufferParams.byteSize = params.entryByteStride * params.capacity;
        bufferParams.name = tableNames[(size_t)stage];

        auto& shaderTable = mShaderTables[(size_t)stage];
        shaderTable = std::make_shared<Buffer>();
        shaderTable->init(bufferParams);
    }
}

void ShaderTable::addPipelineState(
    std::shared_ptr<RaytracingPipelineState> pipelineState,
    std::array<nonstd::span<const std::byte>, (size_t)RaytracingPipelineStage::Count> localRootArguments,
    ID3D12GraphicsCommandList* commandList)
{
    std::lock_guard lockGuard(mMutex);

    for(auto stage : Enumerator<RaytracingPipelineStage>())
    {
        const auto& stageArguments = localRootArguments[(size_t)stage];
        const auto stageIdentifier = pipelineState->getShaderIdentifier(stage);

        GpuBufferWriteGuard<Buffer> raygenTableWriteGuard{*mShaderTables[(size_t)stage], commandList};
        auto writeBuffer = raygenTableWriteGuard.getWriteBuffer();
        writeBuffer = writeBuffer.subspan(mShaderTableSizes[(size_t)stage] * mParams[(size_t)stage].entryByteStride,
                                          stageArguments.size_bytes());

        auto writeItr = writeBuffer.begin();
        writeItr = std::copy(stageIdentifier.begin(), stageIdentifier.end(), writeItr);
        std::copy(stageArguments.begin(), stageArguments.end(), writeItr);

        ++mShaderTableSizes[(size_t)stage];
    }

    auto result = mPipelineStates.emplace(std::move(pipelineState));
    if(!result.second) { ++result.first->refCount; }
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE ShaderTable::getRaygenRecordAddressAndStride(size_t index) const
{
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE ret;
    ret.SizeInBytes = mParams[(size_t)RaytracingPipelineStage::RayGen].entryByteStride;
    ret.StartAddress = mShaderTables[(size_t)RaytracingPipelineStage::RayGen]->getResource()->GetGPUVirtualAddress() +
                       ret.SizeInBytes * index;

    return ret;
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ShaderTable::getHitGroupTableAddressRangeAndStride(size_t indexOffset,
                                                                                              size_t count) const
{
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ret;
    ret.StrideInBytes = mParams[(size_t)RaytracingPipelineStage::HitGroup].entryByteStride;
    ret.StartAddress = mShaderTables[(size_t)RaytracingPipelineStage::HitGroup]->getResource()->GetGPUVirtualAddress() +
                       ret.StrideInBytes * indexOffset;
    ret.SizeInBytes = (count > 0) ? ret.StrideInBytes * count
                                  : mShaderTables[(size_t)RaytracingPipelineStage::HitGroup]->getByteSize() -
                                        (ret.StrideInBytes * indexOffset);

    return ret;
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ShaderTable::getMissTableAddressRangeAndStride(size_t indexOffset,
                                                                                          size_t count) const
{
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ret;
    ret.StrideInBytes = mParams[(size_t)RaytracingPipelineStage::Miss].entryByteStride;
    ret.StartAddress = mShaderTables[(size_t)RaytracingPipelineStage::Miss]->getResource()->GetGPUVirtualAddress() +
                       ret.StrideInBytes * indexOffset;
    ret.SizeInBytes = (count > 0) ? ret.StrideInBytes * count
                                  : mShaderTables[(size_t)RaytracingPipelineStage::Miss]->getByteSize() -
                                        (ret.StrideInBytes * indexOffset);

    return ret;
}

void ShaderTable::markAsUsed(ID3D12CommandList* commandList)
{
    for(auto& shaderTable : mShaderTables)
    {
        shaderTable->markAsUsed(commandList);
    }

    for(auto& pipelineState : mPipelineStates)
    {
        pipelineState.pipelineState->markAsUsed(commandList);
    }
}

void ShaderTable::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    for(auto& shaderTable : mShaderTables)
    {
        shaderTable->markAsUsed(commandQueue);
    }

    for(auto& pipelineState : mPipelineStates)
    {
        pipelineState.pipelineState->markAsUsed(commandQueue);
    }
}

} // namespace scrap::d3d12