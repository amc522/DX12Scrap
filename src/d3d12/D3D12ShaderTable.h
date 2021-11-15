#pragma once

#include "RenderDefs.h"

#include <memory>
#include <mutex>
#include <string_view>
#include <vector>

#include <EASTL/vector_set.h>
#include <nonstd/span.hpp>

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

class ShaderTable
{
public:
    ShaderTable() = default;

    void init(const ShaderTableParams& params);

    void addPipelineState(
        std::shared_ptr<RaytracingPipelineState> pipelineState,
        std::array<nonstd::span<const std::byte>, (size_t)RaytracingPipelineStage::Count> localRootArguments,
        ID3D12GraphicsCommandList* commandList);

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE getRaygenRecordAddressAndStride(size_t index) const;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getHitGroupTableAddressRangeAndStride(size_t indexOffset,
                                                                                     size_t count = 0) const;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getMissTableAddressRangeAndStride(size_t indexOffset,
                                                                                 size_t count = 0) const;

    void markAsUsed(ID3D12CommandList* commandList);
    void markAsUsed(ID3D12CommandQueue* commandQueue);

private:
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

    std::array<std::shared_ptr<Buffer>, (size_t)RaytracingPipelineStage::Count> mShaderTables;
    std::array<uint32_t, (size_t)RaytracingPipelineStage::Count> mShaderTableSizes = {};
    std::array<ShaderTableStageParams, (size_t)RaytracingPipelineStage::Count> mParams;
    eastl::vector_set<RefCountedPipelineState> mPipelineStates;
    std::mutex mMutex;
};
} // namespace scrap::d3d12