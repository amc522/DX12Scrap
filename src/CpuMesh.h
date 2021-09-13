#pragma once

#include "FormattedBuffer.h"
#include "RenderDefs.h"
#include "d3d12/D3D12Buffer.h"

#include <gpufmt/format.h>
#include <nonstd/span.hpp>

namespace scrap
{
class CpuMesh
{
public:
    struct VertexElement
    {
        ShaderVertexSemantic semantic = ShaderVertexSemantic::Unknown;
        uint32_t semanticIndex = 0;
        gpufmt::Format format = gpufmt::Format::UNDEFINED;
        std::vector<std::byte> data;
    };

    struct IndexBuffer
    {
        IndexBufferFormat format;
        std::vector<std::byte> data;
        uint32_t indexCount = 0;
    };

    CpuMesh() = default;
    CpuMesh(PrimitiveTopology topology);

    [[nodiscard]] PrimitiveTopology getPrimitiveTopology() const { return mPrimitiveTopology; }

    void initIndices(IndexBufferFormat format, uint32_t indexCount);
    void initIndices(IndexBufferFormat format, uint32_t indexCount, nonstd::span<std::byte> data);

    [[nodiscard]] IndexBuffer getIndexBuffer() const;

    [[nodiscard]] FormattedBufferSpan accessIndices()
    {
        return FormattedBufferSpan{TranslateIndexBufferFormat(mIndexBuffer.format), mIndexBuffer.data};
    }

    [[nodiscard]] FormattedBufferView getIndices() const
    {
        return FormattedBufferView{TranslateIndexBufferFormat(mIndexBuffer.format), mIndexBuffer.data};
    }

    void createVertexElement(ShaderVertexSemantic semantic,
                             uint32_t semanticIndex,
                             gpufmt::Format format,
                             uint32_t elementCount);
    void createVertexElement(ShaderVertexSemantic semantic,
                             uint32_t semanticIndex,
                             gpufmt::Format format,
                             uint32_t elementCount,
                             nonstd::span<const std::byte> data);

    [[nodiscard]] const VertexElement* getVertexElement(ShaderVertexSemantic semantic, uint32_t semanticIndex) const;

    [[nodiscard]] FormattedBufferSpan accessVertexElementBuffer(ShaderVertexSemantic semantic, uint32_t semanticIndex);
    [[nodiscard]] FormattedBufferView getVertexElementBuffer(ShaderVertexSemantic semantic, uint32_t semanticIndex);

    [[nodiscard]] nonstd::span<VertexElement> accessVertexElements() { return mVertexElements; }

    [[nodiscard]] nonstd::span<const VertexElement> getVertexElements() const { return mVertexElements; }

private:
    PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::Undefined;
    IndexBuffer mIndexBuffer;
    std::vector<VertexElement> mVertexElements;
};

} // namespace scrap