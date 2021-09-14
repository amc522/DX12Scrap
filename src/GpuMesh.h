#pragma once

#include "RenderDefs.h"
#include "d3d12/D3D12Buffer.h"

#include <memory>
#include <string_view>
#include <vector>

#include <nonstd/span.hpp>

namespace scrap
{
class CpuMesh;

class GpuMesh
{
public:
    struct VertexElement
    {
        ShaderVertexSemantic semantic;
        uint32_t semanticIndex;
        std::shared_ptr<d3d12::Buffer> buffer;
    };

    GpuMesh() = default;
    GpuMesh(PrimitiveTopology topology);
    GpuMesh(const CpuMesh& cpuMesh, ResourceAccessFlags accessFlags, std::string_view name);

    [[nodiscard]] PrimitiveTopology getPrimitiveTopology() const { return mPrimitiveTopology; }

    void initIndices(IndexBufferFormat format,
                     uint32_t elementCount,
                     ResourceAccessFlags accessFlags,
                     std::string_view name);
    void initIndices(IndexBufferFormat format,
                     uint32_t elementCount,
                     ResourceAccessFlags accessFlags,
                     std::string_view name,
                     nonstd::span<const std::byte> data);

    [[nodiscard]] const std::shared_ptr<d3d12::Buffer> getIndexBuffer() const { return mIndexBuffer; }

    [[nodiscard]] uint32_t getIndexCount() const { return mIndexCount; }
    void createVertexElement(ShaderVertexSemantic semantic,
                             uint32_t semanticIndex,
                             const d3d12::Buffer::FormattedParams& bufferParams);
    void createVertexElement(ShaderVertexSemantic semantic,
                             uint32_t semanticIndex,
                             const d3d12::Buffer::FormattedParams& bufferParams,
                             nonstd::span<const std::byte> data);

    [[nodiscard]] const std::shared_ptr<d3d12::Buffer>& getVertexBuffer(ShaderVertexSemantic semantic,
                                                                        uint32_t semanticIndex) const;

    [[nodiscard]] nonstd::span<const VertexElement> getVertexElements() const { return mVertexElements; }

    [[nodiscard]] bool isReady() const;
    void markAsUsed(ID3D12CommandList* commandList);

private:
    PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::Undefined;
    std::shared_ptr<d3d12::Buffer> mIndexBuffer;
    std::vector<VertexElement> mVertexElements;
    uint32_t mIndexCount = 0;
};
} // namespace scrap