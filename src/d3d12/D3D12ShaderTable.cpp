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
    mParams[(size_t)RaytracingPipelineStage::HitGroup] = params.hitGroup;
    mParams[(size_t)RaytracingPipelineStage::Miss] = params.miss;

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

        auto& stageShaderTable = mShaderTables[(size_t)stage];
        stageShaderTable.shaderTableBuffer = std::make_shared<Buffer>();
        stageShaderTable.shaderTableBuffer->init(bufferParams);
        stageShaderTable.freeBlocks = FreeBlockTracker(params.capacity);
    }
}

tl::expected<ShaderTableAllocation, ShaderTable::Error> ShaderTable::addPipelineState(
    std::shared_ptr<RaytracingPipelineState> pipelineState,
    std::array<std::span<const std::byte>, (size_t)RaytracingPipelineStage::Count> localRootArguments,
    ID3D12GraphicsCommandList* commandList)
{
    ShaderTableAllocation allocation;
    allocation.mShaderTable = this;

    std::lock_guard lockGuard(mMutex);

    for(auto stage : Enumerator<RaytracingPipelineStage>())
    {
        const auto& stageArguments = localRootArguments[(size_t)stage];
        const auto stageIdentifier = pipelineState->getShaderIdentifier(stage);

        StageShaderTable& stageShaderTable = mShaderTables[(size_t)stage];
        auto result = stageShaderTable.freeBlocks.reserve();

        if(!result) { return tl::make_unexpected(Error::InsufficientSpace); }

        GpuBufferWriteGuard<Buffer> stageTableWriteGuard{*stageShaderTable.shaderTableBuffer, commandList};
        auto writeBuffer = stageTableWriteGuard.getWriteBuffer();
        writeBuffer = writeBuffer.subspan(result->getRange().start * mParams[(size_t)stage].entryByteStride,
                                          detail::kShaderIdentifierSize + stageArguments.size_bytes());

        auto writeItr = writeBuffer.begin();
        writeItr = std::copy(stageIdentifier.begin(), stageIdentifier.end(), writeItr);
        std::copy(stageArguments.begin(), stageArguments.end(), writeItr);

        allocation.mTableReservations[(size_t)stage] = std::move(result.value());
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

void ShaderTable::updateLocalRootArguments(RaytracingPipelineStage stage,
                                           size_t index,
                                           std::span<const std::byte> localRootArguments,
                                           ID3D12GraphicsCommandList* commandList)
{
    std::shared_ptr<d3d12::Buffer>& buffer = mShaderTables[(size_t)stage].shaderTableBuffer;

    GpuBufferWriteGuard<d3d12::Buffer> writeGuard(*buffer, commandList);

    const size_t entryByteSize = mParams[(size_t)stage].entryByteStride;
    const size_t offset = index * entryByteSize + detail::kShaderIdentifierSize;
    auto rootArgumentsBuffer =
        writeGuard.getWriteBuffer().subspan(offset, entryByteSize - detail::kShaderIdentifierSize);
    std::copy(localRootArguments.begin(), localRootArguments.end(), rootArgumentsBuffer.begin());
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE ShaderTable::getRaygenRecordAddressAndStride(size_t index) const
{
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE ret;
    ret.SizeInBytes = mParams[(size_t)RaytracingPipelineStage::RayGen].entryByteStride;
    ret.StartAddress = mShaderTables[(size_t)RaytracingPipelineStage::RayGen]
                           .shaderTableBuffer->getResource()
                           ->GetGPUVirtualAddress() +
                       ret.SizeInBytes * index;

    return ret;
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ShaderTable::getHitGroupTableAddressRangeAndStride(size_t indexOffset,
                                                                                              size_t count) const
{
    const std::shared_ptr<Buffer>& shaderTableBuffer =
        mShaderTables[(size_t)RaytracingPipelineStage::HitGroup].shaderTableBuffer;

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ret;
    ret.StrideInBytes = mParams[(size_t)RaytracingPipelineStage::HitGroup].entryByteStride;
    ret.StartAddress = shaderTableBuffer->getResource()->GetGPUVirtualAddress() + ret.StrideInBytes * indexOffset;
    ret.SizeInBytes =
        (count > 0) ? ret.StrideInBytes * count : shaderTableBuffer->getByteSize() - (ret.StrideInBytes * indexOffset);

    return ret;
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ShaderTable::getMissTableAddressRangeAndStride(size_t indexOffset,
                                                                                          size_t count) const
{
    const std::shared_ptr<Buffer>& shaderTableBuffer =
        mShaderTables[(size_t)RaytracingPipelineStage::Miss].shaderTableBuffer;

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ret;
    ret.StrideInBytes = mParams[(size_t)RaytracingPipelineStage::Miss].entryByteStride;
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
    mShaderTable->updateLocalRootArguments(stage, mTableReservations[(size_t)stage].getRange().start,
                                           localRootArguments, commandList);
}

void ShaderTableAllocation::destroy()
{
    if(mShaderTable == nullptr) { return; }

    mShaderTable->deallocate(mPipelineState);
}

} // namespace scrap::d3d12