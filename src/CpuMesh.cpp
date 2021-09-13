#include "CpuMesh.h"

#include <gpufmt/traits.h>

namespace scrap
{
CpuMesh::CpuMesh(PrimitiveTopology topology): mPrimitiveTopology(topology) {}

void CpuMesh::initIndices(IndexBufferFormat format, uint32_t indexCount)
{
    mIndexBuffer.format = format;
    mIndexBuffer.data.resize(IndexBufferFormatByteSize(format) * indexCount);
}

void CpuMesh::initIndices(IndexBufferFormat format, uint32_t indexCount, nonstd::span<std::byte> data)
{
    mIndexBuffer.format = format;
    mIndexBuffer.data.resize(IndexBufferFormatByteSize(format) * indexCount);
    mIndexBuffer.indexCount = indexCount;

    std::copy(data.begin(), data.end(), mIndexBuffer.data.begin());
}

CpuMesh::IndexBuffer CpuMesh::getIndexBuffer() const
{
    return mIndexBuffer;
}

void CpuMesh::createVertexElement(ShaderVertexSemantic semantic,
                                  uint32_t semanticIndex,
                                  gpufmt::Format format,
                                  uint32_t elementCount)
{
    auto elementItr = std::find_if(mVertexElements.begin(), mVertexElements.end(),
                                   [semantic, semanticIndex](const VertexElement& element) {
                                       return element.semantic == semantic && element.semanticIndex == semanticIndex;
                                   });

    if(elementItr == mVertexElements.end())
    {
        mVertexElements.emplace_back(VertexElement{semantic, semanticIndex, format});
        elementItr = mVertexElements.end() - 1;
    }

    elementItr->data.resize(gpufmt::formatInfo(format).blockByteSize * elementCount);
}

void CpuMesh::createVertexElement(ShaderVertexSemantic semantic,
                                  uint32_t semanticIndex,
                                  gpufmt::Format format,
                                  uint32_t elementCount,
                                  nonstd::span<const std::byte> data)
{
    auto elementItr = std::find_if(mVertexElements.begin(), mVertexElements.end(),
                                   [semantic, semanticIndex](const VertexElement& element) {
                                       return element.semantic == semantic && element.semanticIndex == semanticIndex;
                                   });

    if(elementItr == mVertexElements.end())
    {
        mVertexElements.emplace_back(VertexElement{semantic, semanticIndex, format});
        elementItr = mVertexElements.end() - 1;
    }

    elementItr->data.resize(gpufmt::formatInfo(format).blockByteSize * elementCount);
    std::copy(data.begin(), data.end(), elementItr->data.begin());
}

const CpuMesh::VertexElement* CpuMesh::getVertexElement(ShaderVertexSemantic semantic, uint32_t semanticIndex) const
{
    auto elementItr = std::find_if(mVertexElements.cbegin(), mVertexElements.cend(),
                                   [semantic, semanticIndex](const VertexElement& element) {
                                       return element.semantic == semantic && element.semanticIndex == semanticIndex;
                                   });

    if(elementItr == mVertexElements.cend()) { return nullptr; }

    return &(*elementItr);
}

FormattedBufferSpan CpuMesh::accessVertexElementBuffer(ShaderVertexSemantic semantic, uint32_t semanticIndex)
{
    for(VertexElement& element : mVertexElements)
    {
        if(element.semantic == semantic && element.semanticIndex == semanticIndex)
        {
            return {element.format, element.data};
        }
    }

    return {};
}

FormattedBufferView CpuMesh::getVertexElementBuffer(ShaderVertexSemantic semantic, uint32_t semanticIndex)
{
    for(VertexElement& element : mVertexElements)
    {
        if(element.semantic == semantic && element.semanticIndex == semanticIndex)
        {
            return {element.format, element.data};
        }
    }

    return {};
}
} // namespace scrap