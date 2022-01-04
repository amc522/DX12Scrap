#pragma once

#include "CpuMesh.h"
#include "FormattedBuffer.h"

#include <limits>
#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <gpufmt/format.h>

namespace scrap
{
struct MeshSizes
{
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
};

struct PrimitiveMesh2dParams
{
    std::span<std::byte> indices;
    std::span<std::byte> positions;
    std::span<std::byte> uvs;
};

struct PrimitiveMesh3dParams
{
    PrimitiveMesh3dParams() = default;
    PrimitiveMesh3dParams(CpuMesh& mesh)
        : indices(mesh.accessIndices())
        , positions(mesh.accessVertexElementBuffer(ShaderVertexSemantic::Position, 0))
        , normals(mesh.accessVertexElementBuffer(ShaderVertexSemantic::Normal, 0))
        , tangents(mesh.accessVertexElementBuffer(ShaderVertexSemantic::Tangent, 0))
        , binormals(mesh.accessVertexElementBuffer(ShaderVertexSemantic::Binormal, 0))
        , uvs(mesh.accessVertexElementBuffer(ShaderVertexSemantic::TexCoord, 0))
    {}

    FormattedBufferSpan indices;
    FormattedBufferSpan positions;
    FormattedBufferSpan normals;
    FormattedBufferSpan tangents;
    FormattedBufferSpan binormals;
    FormattedBufferSpan uvs;
};

enum class CubeMeshTopologyType
{
    Triangle,
    Quad,
};

constexpr MeshSizes CalculateCubeMeshSizes(CubeMeshTopologyType topologyType, uint32_t subdivisions = 0)
{
    MeshSizes sizes;
    sizes.vertexCount = (2 + subdivisions) * (2 + subdivisions) * 6;
    sizes.indexCount =
        (topologyType == CubeMeshTopologyType::Triangle)
            ? (1 + subdivisions) * (1 + subdivisions) * 6 * 6  // 6 indices per quad (2 triangles), 6 faces
            : (1 + subdivisions) * (1 + subdivisions) * 4 * 6; // 4 indices per quad, 6 faces

    return sizes;
}

constexpr PrimitiveTopology GetCubeMeshPrimitiveTopology(CubeMeshTopologyType topologyType)
{
    switch(topologyType)
    {
    case scrap::CubeMeshTopologyType::Triangle: return PrimitiveTopology::TriangleList;
    case scrap::CubeMeshTopologyType::Quad: return PrimitiveTopology::Patch_4ControlPoint;
    default: assert(false); return PrimitiveTopology::Undefined;
    }
}

MeshSizes GenerateCubeMeshTris(PrimitiveMesh3dParams& mesh,
                               uint32_t subdivisions = 0,
                               glm::vec3 offset = {0.0f, 0.0f, 0.0f},
                               glm::vec3 size = {1.0f, 1.0f, 1.0f});

MeshSizes GenerateCubeMeshQuads(PrimitiveMesh3dParams& mesh,
                                uint32_t subdivisions = 0,
                                glm::vec3 offset = {0.0f, 0.0f, 0.0f},
                                glm::vec3 size = {1.0f, 1.0f, 1.0f});

MeshSizes GenerateCubeMesh(CubeMeshTopologyType topologyType,
                           PrimitiveMesh3dParams& mesh,
                           uint32_t subdivisions = 0,
                           glm::vec3 offset = {0.0f, 0.0f, 0.0f},
                           glm::vec3 size = {1.0f, 1.0f, 1.0f});

CpuMesh GenerateCubeMesh(CubeMeshTopologyType topologyType,
                         uint32_t subdivisions = 0,
                         glm::vec3 offset = {0.0f, 0.0f, 0.0f},
                         glm::vec3 size = {1.0f, 1.0f, 1.0f});
} // namespace scrap