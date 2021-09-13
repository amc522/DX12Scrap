#include "GpuMesh.h"

#include "CpuMesh.h"

#include <fmt/format.h>
#include <gpufmt/traits.h>

namespace scrap
{
GpuMesh::GpuMesh(PrimitiveTopology topology): mPrimitiveTopology(topology) {}

GpuMesh::GpuMesh(const CpuMesh& cpuMesh, ResourceAccessFlags accessFlags, std::string_view name)
    : mPrimitiveTopology(cpuMesh.getPrimitiveTopology())
{
    std::string bufferName;

    {
        fmt::format_to(std::back_inserter(bufferName), "{}_Indices", name);

        initIndices(cpuMesh.getIndexBuffer().format, (uint32_t)cpuMesh.getIndices().elementCount(), accessFlags,
                    bufferName, cpuMesh.getIndices().buffer);
    }

    nonstd::span vertexElements = cpuMesh.getVertexElements();
    mVertexElements.reserve(vertexElements.size());

    for(const CpuMesh::VertexElement& element : vertexElements)
    {
        bufferName.clear();
        fmt::format_to(std::back_inserter(bufferName), "{}_{}{}", name, element.semantic, element.semanticIndex);

        d3d12::Buffer::FormattedParams params;
        params.accessFlags = accessFlags;
        params.format = element.format;
        params.numElements =
            static_cast<uint32_t>(element.data.size() / gpufmt::formatInfo(element.format).blockByteSize);
        params.name = bufferName;
        createVertexElement(element.semantic, element.semanticIndex, params, element.data);
    }
}

void GpuMesh::initIndices(IndexBufferFormat format,
                          uint32_t elementCount,
                          ResourceAccessFlags accessFlags,
                          std::string_view name)
{
    d3d12::Buffer::FormattedParams params;
    params.accessFlags = accessFlags;
    params.format = TranslateIndexBufferFormat(format);
    params.numElements = elementCount;
    params.name = name;

    mIndexBuffer = std::make_shared<d3d12::Buffer>();
    mIndexBuffer->init(params);

    mIndexCount = elementCount;
}

void GpuMesh::initIndices(IndexBufferFormat format,
                          uint32_t elementCount,
                          ResourceAccessFlags accessFlags,
                          std::string_view name,
                          nonstd::span<const std::byte> data)
{
    d3d12::Buffer::FormattedParams params;
    params.accessFlags = accessFlags;
    params.format = TranslateIndexBufferFormat(format);
    params.numElements = elementCount;
    params.name = name;

    mIndexBuffer = std::make_shared<d3d12::Buffer>();
    mIndexBuffer->init(params, data);

    mIndexCount = elementCount;
}

void GpuMesh::createVertexElement(ShaderVertexSemantic semantic,
                                  uint32_t semanticIndex,
                                  const d3d12::Buffer::FormattedParams& bufferParams)
{
    if(bufferParams.numElements == 0) { return; }

    std::vector<VertexElement>::iterator elementItr;

    elementItr = std::find_if(mVertexElements.begin(), mVertexElements.end(),
                              [semantic, semanticIndex](const VertexElement& element) {
                                  return element.semantic == semantic && element.semanticIndex == semanticIndex;
                              });

    if(elementItr == mVertexElements.end())
    {
        mVertexElements.emplace_back();
        elementItr = mVertexElements.end() - 1;
    }

    elementItr->semantic = semantic;
    elementItr->semanticIndex = semanticIndex;

    elementItr->buffer = std::make_shared<d3d12::Buffer>();
    elementItr->buffer->init(bufferParams);
}

void GpuMesh::createVertexElement(ShaderVertexSemantic semantic,
                                  uint32_t semanticIndex,
                                  const d3d12::Buffer::FormattedParams& bufferParams,
                                  nonstd::span<const std::byte> data)
{
    if(bufferParams.numElements == 0) { return; }

    const size_t bufferByteSize = bufferParams.numElements * gpufmt::formatInfo(bufferParams.format).blockByteSize;
    if(data.size_bytes() > bufferByteSize) { return; }

    std::vector<VertexElement>::iterator elementItr;

    elementItr = std::find_if(mVertexElements.begin(), mVertexElements.end(),
                              [semantic, semanticIndex](const VertexElement& element) {
                                  return element.semantic == semantic && element.semanticIndex == semanticIndex;
                              });

    if(elementItr == mVertexElements.end())
    {
        mVertexElements.emplace_back();
        elementItr = mVertexElements.end() - 1;
    }

    elementItr->semantic = semantic;
    elementItr->semanticIndex = semanticIndex;

    elementItr->buffer = std::make_shared<d3d12::Buffer>();
    elementItr->buffer->init(bufferParams, data);
}

const std::shared_ptr<d3d12::Buffer>& GpuMesh::getVertexBuffer(ShaderVertexSemantic semantic,
                                                               uint32_t semanticIndex) const
{
    for(const auto& element : mVertexElements)
    {
        if(element.semantic == semantic && element.semanticIndex == semanticIndex) { return element.buffer; }
    }

    static std::shared_ptr<d3d12::Buffer> empty;
    return empty;
}

bool GpuMesh::isReady() const
{
    if(mIndexBuffer != nullptr && !mIndexBuffer->isReady()) { return false; }

    for(const auto& element : mVertexElements)
    {
        if(!element.buffer->isReady()) { return false; }
    }

    return true;
}

void GpuMesh::markAsUsed(ID3D12CommandList* commandList)
{
    if(mIndexBuffer != nullptr) { mIndexBuffer->markAsUsed(commandList); }

    for(auto& element : mVertexElements)
    {
        element.buffer->markAsUsed(commandList);
    }
}
} // namespace scrap