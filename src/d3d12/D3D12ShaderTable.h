#pragma once

#include "FreeBlockTracker.h"
#include "RenderDefs.h"

#include <memory>
#include <mutex>
#include <span>
#include <string_view>
#include <vector>

#include <EASTL/vector_set.h>

struct D3D12_GPU_VIRTUAL_ADDRESS_RANGE;
struct D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE;
struct ID3D12CommandList;
struct ID3D12CommandQueue;
struct ID3D12GraphicsCommandList;

namespace scrap::d3d12
{
class Buffer;
class RaytracingPipelineState;

struct ShaderTableStageParams
{
    size_t entryByteStride = 0;
    size_t capacity = 0;
};

struct ShaderTableParams
{
    ShaderTableStageParams raygen;
    ShaderTableStageParams hitGroup;
    ShaderTableStageParams miss;
    std::string_view name;
};

class ShaderTable;

class ShaderTableAllocation
{
public:
    ShaderTableAllocation() = default;
    ShaderTableAllocation(const ShaderTableAllocation&) = delete;
    ShaderTableAllocation(ShaderTableAllocation&& other)
        : mShaderTable(other.mShaderTable)
        , mTableReservations(std::move(other.mTableReservations))
        , mPipelineState(std::move(other.mPipelineState))
    {
        other.mShaderTable = nullptr;
    }

    ~ShaderTableAllocation() { destroy(); }

    ShaderTableAllocation& operator=(const ShaderTableAllocation&) = delete;
    ShaderTableAllocation& operator=(ShaderTableAllocation&& other);

    void updateLocalRootArguments(RaytracingPipelineStage stage,
                                  std::span<const std::byte> localRootArguments,
                                  ID3D12GraphicsCommandList* commandList);

private:
    friend class ShaderTable;

    void destroy();

    ShaderTable* mShaderTable = nullptr;
    std::array<ScopedReservedBlocks, (size_t)RaytracingPipelineStage::Count> mTableReservations = {};
    std::shared_ptr<RaytracingPipelineState> mPipelineState;
};

class ShaderTable
{
public:
    enum class Error
    {
        InsufficientSpace
    };

    ShaderTable() = default;

    void init(const ShaderTableParams& params);

    [[nodiscard]] tl::expected<ShaderTableAllocation, Error>
    addPipelineState(std::shared_ptr<RaytracingPipelineState> pipelineState,
                     std::array<std::span<const std::byte>, (size_t)RaytracingPipelineStage::Count> localRootArguments,
                     ID3D12GraphicsCommandList* commandList);

    void deallocate(const std::shared_ptr<RaytracingPipelineState>& pipelineState);

    void updateLocalRootArguments(RaytracingPipelineStage stage,
                                  size_t index,
                                  std::span<const std::byte> localRootArguments,
                                  ID3D12GraphicsCommandList* commandList);

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE getRaygenRecordAddressAndStride(size_t index) const;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getHitGroupTableAddressRangeAndStride(size_t indexOffset,
                                                                                     size_t count = 0) const;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getMissTableAddressRangeAndStride(size_t indexOffset,
                                                                                 size_t count = 0) const;

    void markAsUsed(ID3D12CommandList* commandList);
    void markAsUsed(ID3D12CommandQueue* commandQueue);

private:
    struct StageShaderTable
    {
        std::shared_ptr<Buffer> shaderTableBuffer;
        FreeBlockTracker freeBlocks;
    };

    struct RefCountedPipelineState
    {
        RefCountedPipelineState(std::shared_ptr<RaytracingPipelineState> pipelineState_)
            : pipelineState(std::move(pipelineState_))
        {}

        std::shared_ptr<RaytracingPipelineState> pipelineState;
        int refCount = 1;

        [[nodiscard]] bool operator==(const RefCountedPipelineState& right) const noexcept
        {
            return pipelineState == right.pipelineState;
        }

        [[nodiscard]] bool operator<(const RefCountedPipelineState& right) const noexcept
        {
            return pipelineState < right.pipelineState;
        }
    };

    std::array<StageShaderTable, (size_t)RaytracingPipelineStage::Count> mShaderTables;
    std::array<ShaderTableStageParams, (size_t)RaytracingPipelineStage::Count> mParams;
    eastl::vector_set<RefCountedPipelineState> mPipelineStates;
    std::mutex mMutex;
};
} // namespace scrap::d3d12