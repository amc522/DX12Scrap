#pragma once

#include "RenderDefs.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12VertexBuffer.h"

#include <memory>
#include <span>
#include <string_view>
#include <vector>

namespace scrap
{
class CpuMesh;

class GpuMesh
{
public:
    struct IndexBufferParams
    {
        ResourceAccessFlags accessFlags = ResourceAccessFlags::None;
        d3d12::BufferFlags flags = d3d12::BufferFlags::None;
        std::optional<D3D12_RESOURCE_STATES> initialResourceState;
        std::string_view name;
        IndexBufferFormat format = IndexBufferFormat::UInt32;
        uint32_t numIndices = 0;
    };

    GpuMesh() = default;
    GpuMesh(PrimitiveTopology topology);
    GpuMesh(const CpuMesh& cpuMesh, ResourceAccessFlags accessFlags, std::string_view name);

    [[nodiscard]] PrimitiveTopology getPrimitiveTopology() const { return mPrimitiveTopology; }

    void initIndices(const IndexBufferParams& params);
    void initIndices(const IndexBufferParams& params, std::span<const std::byte> data);

    [[nodiscard]] const std::shared_ptr<d3d12::Buffer> getIndexBuffer() const { return mIndexBuffer; }

    [[nodiscard]] uint32_t getIndexCount() const { return mIndexCount; }
    void createVertexElement(ShaderVertexSemantic semantic,
                             uint32_t semanticIndex,
                             const d3d12::Buffer::FormattedParams& bufferParams);
    void createVertexElement(ShaderVertexSemantic semantic,
                             uint32_t semanticIndex,
                             const d3d12::Buffer::FormattedParams& bufferParams,
                             std::span<const std::byte> data);

    [[nodiscard]] const std::shared_ptr<d3d12::Buffer>& getVertexBuffer(ShaderVertexSemantic semantic,
                                                                        uint32_t semanticIndex) const;

    [[nodiscard]] std::span<const d3d12::VertexBuffer> getVertexElements() const { return mVertexElements; }

    [[nodiscard]] bool isReady() const;
    void markAsUsed(ID3D12CommandList* commandList);

private:
    PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::Undefined;
    std::shared_ptr<d3d12::Buffer> mIndexBuffer;
    std::vector<d3d12::VertexBuffer> mVertexElements;
    uint32_t mIndexCount = 0;
};
} // namespace scrap