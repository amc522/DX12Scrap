#include "GpuMesh.h"

#include "CpuMesh.h"
#include "d3d12/D3D12BLAccelerationStructure.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12VertexBuffer.h"

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

        IndexBufferParams indexParams;
        indexParams.accessFlags = accessFlags;
        indexParams.format = cpuMesh.getIndexBuffer().format;
        indexParams.name = bufferName;
        indexParams.numIndices = (uint32_t)cpuMesh.getIndices().elementCount();

        initIndices(indexParams, cpuMesh.getIndices().buffer);
    }

    std::span vertexElements = cpuMesh.getVertexElements();
    mVertexElements.reserve(vertexElements.size());

    for(const CpuMesh::VertexElement& element : vertexElements)
    {
        bufferName.clear();
        fmt::format_to(std::back_inserter(bufferName), "{}_{}{}", name, element.semantic, element.semanticIndex);

        d3d12::BufferFormattedParams params;
        params.accessFlags = accessFlags;
        params.format = element.format;
        params.numElements =
            static_cast<uint32_t>(element.data.size() / gpufmt::formatInfo(element.format).blockByteSize);
        params.name = bufferName;
        createVertexElement(element.semantic, element.semanticIndex, params, element.data);
    }

    createBlas();
}

void GpuMesh::initIndices(const IndexBufferParams& params)
{
    d3d12::BufferFormattedParams bufferParams;
    bufferParams.accessFlags = params.accessFlags;
    bufferParams.flags = params.flags | d3d12::BufferFlags::IndexBuffer;
    bufferParams.format = TranslateIndexBufferFormat(params.format);
    bufferParams.numElements = params.numIndices;
    bufferParams.name = params.name;
    bufferParams.initialResourceState = params.initialResourceState;

    mIndexBuffer = std::make_shared<d3d12::Buffer>();
    mIndexBuffer->init(bufferParams);

    mIndexCount = params.numIndices;
}

void GpuMesh::initIndices(const IndexBufferParams& params, std::span<const std::byte> data)
{
    d3d12::BufferFormattedParams bufferParams;
    bufferParams.accessFlags = params.accessFlags;
    bufferParams.flags = params.flags | d3d12::BufferFlags::IndexBuffer;
    bufferParams.format = TranslateIndexBufferFormat(params.format);
    bufferParams.numElements = params.numIndices;
    bufferParams.name = params.name;
    bufferParams.initialResourceState = params.initialResourceState;

    mIndexBuffer = std::make_shared<d3d12::Buffer>();
    mIndexBuffer->init(bufferParams, data);

    mIndexCount = params.numIndices;
}

void GpuMesh::createVertexElement(ShaderVertexSemantic semantic,
                                  uint32_t semanticIndex,
                                  const d3d12::BufferFormattedParams& bufferParams)
{
    if(bufferParams.numElements == 0) { return; }

    std::vector<d3d12::VertexBuffer>::iterator elementItr;

    elementItr = std::find_if(mVertexElements.begin(), mVertexElements.end(),
                              [semantic, semanticIndex](const d3d12::VertexBuffer& element) {
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
                                  const d3d12::BufferFormattedParams& bufferParams,
                                  std::span<const std::byte> data)
{
    if(bufferParams.numElements == 0) { return; }

    const size_t bufferByteSize = bufferParams.numElements * gpufmt::formatInfo(bufferParams.format).blockByteSize;
    if(data.size_bytes() > bufferByteSize) { return; }

    std::vector<d3d12::VertexBuffer>::iterator elementItr;

    elementItr = std::find_if(mVertexElements.begin(), mVertexElements.end(),
                              [semantic, semanticIndex](const d3d12::VertexBuffer& element) {
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
void GpuMesh::createBlas()
{
    mBlas = std::make_shared<d3d12::BLAccelerationStructure>(
        d3d12::BLAccelerationStructureParams{{.flags = d3d12::AccelerationStructureFlags::None,
                                              .buildOption = d3d12::AccelerationStructureBuildOption::FastTrace,
                                              .initialReservedObjects = 1,
                                              .name = fmt::format("{} blas", mName)}});

    mBlas->addMesh(
        d3d12::BLAccelerationStructureGeometryParams{.indexBuffer = mIndexBuffer,
                                                     .vertexBuffer = getVertexBuffer(ShaderVertexSemantic::Position, 0),
                                                     .flags = d3d12::RaytracingGeometryFlags::None});
}
} // namespace scrap