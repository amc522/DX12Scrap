#include "d3d12/D3D12ShaderTable.h"

#include "EnumArray.h"
#include "EnumIterator.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12CommandList.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12RaytracingPipelineState.h"

#include <d3d12.h>
#include <d3dx12.h>

namespace scrap::d3d12
{
void ShaderTable::init(const ShaderTableParams& params)
{
    mParams[RaytracingPipelineStage::RayGen] = params.raygen;
    mParams[RaytracingPipelineStage::HitGroup] = params.hitGroup;
    mParams[RaytracingPipelineStage::Miss] = params.miss;

    EnumArray<std::string, RaytracingPipelineStage> tableNames{
        fmt::format("{} (Raygen Table)", params.name),
        fmt::format("{} (Hit Group Table)", params.name),
        fmt::format("{} (Miss Table)", params.name),
    };

    BufferSimpleParams bufferParams;
    bufferParams.accessFlags = ResourceAccessFlags::CpuWrite;
    bufferParams.flags = BufferFlags::NonPixelShaderResource;
    bufferParams.initialResourceState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

    for(RaytracingPipelineStage stage : enumerate<RaytracingPipelineStage>())
    {
        ShaderTableStageParams& params = mParams[stage];
        params.entryByteStride = AlignInteger(RaytracingShaderIdentifier::kByteSize + params.entryByteStride,
                                              size_t(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT));

        bufferParams.byteSize = params.entryByteStride * params.capacity;
        bufferParams.name = tableNames[stage];

        auto& stageShaderTable = mShaderTables[stage];
        stageShaderTable.shaderTableBuffer = std::make_shared<Buffer>();
        stageShaderTable.shaderTableBuffer->init(bufferParams);
        stageShaderTable.freeBlocks = FreeBlockTracker(params.capacity);
    }
}

tl::expected<ShaderTableAllocation, ShaderTable::Error>
ShaderTable::addPipelineState(std::shared_ptr<RaytracingPipelineState> pipelineState,
                              EnumArray<std::span<const std::byte>, RaytracingPipelineStage> localRootArguments,
                              ID3D12GraphicsCommandList* commandList)
{
    ShaderTableAllocation allocation;
    allocation.mShaderTable = this;

    std::lock_guard lockGuard(mMutex);

    for(auto stage : enumerate<RaytracingPipelineStage>())
    {
        const auto& stageArguments = localRootArguments[stage];
        const auto& stageIdentifier = pipelineState->getShaderIdentifier(stage);

        if(!stageIdentifier.isInvalid()) { continue; }

        StageShaderTable& stageShaderTable = mShaderTables[stage];
        auto result = stageShaderTable.freeBlocks.reserve();

        if(!result) { return tl::make_unexpected(Error::InsufficientSpace); }

        GpuBufferWriteGuard<Buffer> stageTableWriteGuard{*stageShaderTable.shaderTableBuffer, commandList};
        auto writeBuffer = stageTableWriteGuard.getWriteBuffer();
        writeBuffer = writeBuffer.subspan(result->getRange().start * mParams[stage].entryByteStride,
                                          RaytracingShaderIdentifier::kByteSize + stageArguments.size_bytes());

        auto writeItr = writeBuffer.begin();
        writeItr = std::copy(stageIdentifier.begin(), stageIdentifier.end(), writeItr);
        std::copy(stageArguments.begin(), stageArguments.end(), writeItr);

        allocation.mTableReservations[stage] = std::move(result.value());
    }

    auto result = mPipelineStates.emplace(pipelineState);
    if(!result.second) { ++result.first->refCount; }

    allocation.mPipelineState = std::move(pipelineState);

    return allocation;
}

void ShaderTable::deallocate(const std::shared_ptr<RaytracingPipelineState>& pipelineState)
{
    struct LessThan
    {
        bool operator()(const std::shared_ptr<RaytracingPipelineState>& left, const RefCountedPipelineState& right)
        {
            return left < right.pipelineState;
        }

        bool operator()(const RefCountedPipelineState& left, const std::shared_ptr<RaytracingPipelineState>& right)
        {
            return left.pipelineState < right;
        }
    };

    auto findPtr = mPipelineStates.find_as(pipelineState, LessThan());

    if(findPtr != nullptr)
    {
        --findPtr->refCount;

        if(findPtr->refCount == 0) { mPipelineStates.erase(*findPtr); }
    }
}

void ShaderTable::beginUpdate(GraphicsCommandList& commandList)
{
    EnumArray<D3D12_RESOURCE_BARRIER, RaytracingPipelineStage> barriers;

    for(auto [stage, stageTable] : enumerate(mShaderTables))
    {
        barriers[stage] = CD3DX12_RESOURCE_BARRIER::Transition(
            stageTable.shaderTableBuffer->getResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
    }

    commandList.get()->ResourceBarrier((uint32_t)barriers.size(), barriers.data());

    for(auto stage : enumerate<RaytracingPipelineStage>())
    {
        mShaderTableBufferMaps[stage] =
            GpuBufferWriteGuard<Buffer>(*mShaderTables[stage].shaderTableBuffer, commandList.get());
    }
}

void ShaderTable::endUpdate(GraphicsCommandList& commandList)
{
    for(auto stage : enumerate<RaytracingPipelineStage>())
    {
        mShaderTableBufferMaps[stage] = {};
    }

    EnumArray<D3D12_RESOURCE_BARRIER, RaytracingPipelineStage> barriers;

    for(auto stage : enumerate<RaytracingPipelineStage>())
    {
        const auto& stageTable = mShaderTables[stage];
        barriers[stage] = CD3DX12_RESOURCE_BARRIER::Transition(
            stageTable.shaderTableBuffer->getResource(), D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
    }

    commandList.get()->ResourceBarrier((uint32_t)barriers.size(), barriers.data());
}

void ShaderTable::updateLocalRootArguments(RaytracingPipelineStage stage,
                                           size_t index,
                                           std::span<const std::byte> localRootArguments,
                                           ID3D12GraphicsCommandList* commandList)
{
    std::shared_ptr<d3d12::Buffer>& buffer = mShaderTables[stage].shaderTableBuffer;

    const size_t entryByteSize = mParams[stage].entryByteStride;
    const size_t offset = index * entryByteSize + RaytracingShaderIdentifier::kByteSize;
    auto rootArgumentsBuffer = mShaderTableBufferMaps[stage].getWriteBuffer().subspan(
        offset, entryByteSize - RaytracingShaderIdentifier::kByteSize);
    std::copy(localRootArguments.begin(), localRootArguments.end(), rootArgumentsBuffer.begin());
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE ShaderTable::getRaygenRecordAddressAndStride(size_t index) const
{
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE ret;
    ret.SizeInBytes = mParams[RaytracingPipelineStage::RayGen].entryByteStride;
    ret.StartAddress =
        mShaderTables[RaytracingPipelineStage::RayGen].shaderTableBuffer->getResource()->GetGPUVirtualAddress() +
        ret.SizeInBytes * index;

    return ret;
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ShaderTable::getHitGroupTableAddressRangeAndStride(size_t indexOffset,
                                                                                              size_t count) const
{
    const std::shared_ptr<Buffer>& shaderTableBuffer =
        mShaderTables[RaytracingPipelineStage::HitGroup].shaderTableBuffer;

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ret;
    ret.StrideInBytes = mParams[RaytracingPipelineStage::HitGroup].entryByteStride;
    ret.StartAddress = shaderTableBuffer->getResource()->GetGPUVirtualAddress() + ret.StrideInBytes * indexOffset;
    ret.SizeInBytes =
        (count > 0) ? ret.StrideInBytes * count : shaderTableBuffer->getByteSize() - (ret.StrideInBytes * indexOffset);

    return ret;
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ShaderTable::getMissTableAddressRangeAndStride(size_t indexOffset,
                                                                                          size_t count) const
{
    const std::shared_ptr<Buffer>& shaderTableBuffer = mShaderTables[RaytracingPipelineStage::Miss].shaderTableBuffer;

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ret;
    ret.StrideInBytes = mParams[RaytracingPipelineStage::Miss].entryByteStride;
    ret.StartAddress = shaderTableBuffer->getResource()->GetGPUVirtualAddress() + ret.StrideInBytes * indexOffset;
    ret.SizeInBytes =
        (count > 0) ? ret.StrideInBytes * count : shaderTableBuffer->getByteSize() - (ret.StrideInBytes * indexOffset);

    return ret;
}

void ShaderTable::markAsUsed(ID3D12CommandList* commandList)
{
    for(auto& shaderTable : mShaderTables)
    {
        shaderTable.shaderTableBuffer->markAsUsed(commandList);
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
        shaderTable.shaderTableBuffer->markAsUsed(commandQueue);
    }

    for(auto& pipelineState : mPipelineStates)
    {
        pipelineState.pipelineState->markAsUsed(commandQueue);
    }
}

bool ShaderTable::isReady() const
{
    for(const StageShaderTable& stageShaderTable : mShaderTables)
    {
        if(!stageShaderTable.shaderTableBuffer->isReady()) { return false; }
    }

    return true;
}

ShaderTableAllocation& ShaderTableAllocation::operator=(ShaderTableAllocation&& other)
{
    destroy();

    mShaderTable = other.mShaderTable;
    other.mShaderTable = nullptr;
    mTableReservations = std::move(other.mTableReservations);
    mPipelineState = std::move(other.mPipelineState);

    return *this;
}

void ShaderTableAllocation::updateLocalRootArguments(RaytracingPipelineStage stage,
                                                     std::span<const std::byte> localRootArguments,
                                                     ID3D12GraphicsCommandList* commandList)
{
    mShaderTable->updateLocalRootArguments(stage, mTableReservations[stage].getRange().start, localRootArguments,
                                           commandList);
}

void ShaderTableAllocation::destroy()
{
    if(mShaderTable == nullptr) { return; }

    mShaderTable->deallocate(mPipelineState);
}

} // namespace scrap::d3d12