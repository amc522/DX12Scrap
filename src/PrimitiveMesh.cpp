#include "PrimitiveMesh.h"

#include "AABB.h"

#include <glm/common.hpp>
#include <gpufmt/write.h>

namespace scrap
{
MeshSizes GenerateCubeMeshVertices(PrimitiveMesh3dParams& mesh, uint32_t subdivisions, glm::vec3 offset, glm::vec3 size)
{
    const MeshSizes meshSizes = CalculateCubeMeshSizes(CubeMeshTopologyType::Triangle, subdivisions);

    if(mesh.positions.elementCount() < meshSizes.vertexCount) { return {0, 0}; }

    const uint32_t vertsPerSide = subdivisions + 2;
    const uint32_t vertsPerFace = vertsPerSide * vertsPerSide;
    const AABB bounds(offset, size * 0.5f);

    auto generateFacePos = [&](FormattedBufferSpan& positions, const std::array<glm::vec3, 4>& faceCorners) {
        gpufmt::Writer writer(positions.format);

        if(!writer.isFormatWriteable()) { return; }

        // positions
        for(uint32_t row = 0; row < vertsPerSide; ++row)
        {
            const float rowT = (float)row / (float)(vertsPerSide - 1);
            const glm::vec3 rowStartPos = glm::mix(faceCorners[0], faceCorners[1], rowT);
            const glm::vec3 rowEndPos = glm::mix(faceCorners[3], faceCorners[2], rowT);

            for(uint32_t col = 0; col < vertsPerSide; ++col)
            {
                const uint32_t index = row * vertsPerSide + col;
                const float colT = (float)col / (float)(vertsPerSide - 1);
                const glm::vec4 position =
                    glm::vec4(glm::mix(rowStartPos, rowEndPos, (float)col / (float)(vertsPerSide - 1)), 1.0f);

                writer.writeTo(position, positions.accessElement(index));
            }
        }
    };

    auto generateFaceUv = [&](FormattedBufferSpan& uvs, const std::array<glm::vec3, 4>& faceCorners) {
        gpufmt::Writer writer(uvs.format);

        if(!writer.isFormatWriteable()) { return; }

        for(uint32_t row = 0; row < vertsPerSide; ++row)
        {
            const float rowT = (float)row / (float)(vertsPerSide - 1);

            for(uint32_t col = 0; col < vertsPerSide; ++col)
            {
                const uint32_t index = row * vertsPerSide + col;
                const float colT = (float)col / (float)(vertsPerSide - 1);
                const glm::vec4 uv = glm::vec4(rowT, colT, 0.0f, 0.0f);

                writer.writeTo(uv, uvs.accessElement(index));
            }
        }
    };

    auto fillBasisVector = [&](FormattedBufferSpan& meshVectors, glm::vec3 posX, glm::vec3 negX, glm::vec3 posY,
                               glm::vec3 negY, glm::vec3 posZ, glm::vec3 negZ) {
        gpufmt::Writer writer(meshVectors.format);

        if(!writer.isFormatWriteable()) { return; }

        FormattedBufferSpan faceVectors = meshVectors.formattedSubspan(0, vertsPerFace);
        writer.fill(faceVectors.buffer, glm::vec4(posX, 0.0f));

        faceVectors = meshVectors.formattedSubspan(vertsPerFace, vertsPerFace);
        writer.fill(faceVectors.buffer, glm::vec4(negX, 0.0f));

        faceVectors = meshVectors.formattedSubspan(vertsPerFace * 2, vertsPerFace);
        writer.fill(faceVectors.buffer, glm::vec4(posY, 0.0f));

        faceVectors = meshVectors.formattedSubspan(vertsPerFace * 3, vertsPerFace);
        writer.fill(faceVectors.buffer, glm::vec4(negY, 0.0f));

        faceVectors = meshVectors.formattedSubspan(vertsPerFace * 4, vertsPerFace);
        writer.fill(faceVectors.buffer, glm::vec4(posZ, 0.0f));

        faceVectors = meshVectors.formattedSubspan(vertsPerFace * 5, vertsPerFace);
        writer.fill(faceVectors.buffer, glm::vec4(negZ, 0.0f));
    };

    // positions
    FormattedBufferSpan positions = mesh.positions;
    generateFacePos(positions, bounds.getFacePosX());

    positions = positions.formattedSubspan(vertsPerFace);
    generateFacePos(positions, bounds.getFaceNegX());

    positions = positions.formattedSubspan(vertsPerFace);
    generateFacePos(positions, bounds.getFacePosY());

    positions = positions.formattedSubspan(vertsPerFace);
    generateFacePos(positions, bounds.getFaceNegY());

    positions = positions.formattedSubspan(vertsPerFace);
    generateFacePos(positions, bounds.getFacePosZ());

    positions = positions.formattedSubspan(vertsPerFace);
    generateFacePos(positions, bounds.getFaceNegZ());

    if(mesh.uvs.elementCount() >= meshSizes.vertexCount)
    {
        FormattedBufferSpan uvs = mesh.uvs;
        generateFaceUv(uvs, bounds.getFacePosX());

        uvs = uvs.formattedSubspan(vertsPerFace);
        generateFaceUv(uvs, bounds.getFaceNegX());

        uvs = uvs.formattedSubspan(vertsPerFace);
        generateFaceUv(uvs, bounds.getFacePosY());

        uvs = uvs.formattedSubspan(vertsPerFace);
        generateFaceUv(uvs, bounds.getFaceNegY());

        uvs = uvs.formattedSubspan(vertsPerFace);
        generateFaceUv(uvs, bounds.getFacePosZ());

        uvs = uvs.formattedSubspan(vertsPerFace);
        generateFaceUv(uvs, bounds.getFaceNegZ());
    }

    if(mesh.normals.elementCount() >= meshSizes.vertexCount)
    {
        // clang-format off
        fillBasisVector(mesh.normals,
                        glm::vec3(1.0f, 0.0f, 0.0f),   // +x
                        glm::vec3(-1.0f, 0.0f, 0.0f),  // -x
                        glm::vec3(0.0f, 1.0f, 0.0f),   // +y
                        glm::vec3(0.0f, -1.0f, 0.0f),  // -y
                        glm::vec3(0.0f, 0.0f, 1.0f),   // +z
                        glm::vec3(0.0f, 0.0f, -1.0f)); // -z
        // clang-format on
    }

    if(mesh.tangents.elementCount() >= meshSizes.vertexCount)
    {
        // clang-format off
        fillBasisVector(mesh.tangents,
                        glm::vec3(0.0f, 0.0f, 1.0f),   // +x
                        glm::vec3(0.0f, 0.0f, -1.0f),  // -x
                        glm::vec3(1.0f, 0.0f, 0.0f),   // +y
                        glm::vec3(1.0f, 0.0f, 0.0f),   // -y
                        glm::vec3(-1.0f, 0.0f, 0.0f),  // +z
                        glm::vec3(1.0f, 0.0f, -1.0f)); // -z
        // clang-format on
    }

    if(mesh.binormals.elementCount() >= meshSizes.vertexCount)
    {
        // clang-format off
        fillBasisVector(mesh.binormals,
                        glm::vec3(0.0f, -1.0f, 0.0f),  // +x
                        glm::vec3(0.0f, -1.0f, 0.0f),  // -x
                        glm::vec3(0.0f, 0.0f, -1.0f),  // +y
                        glm::vec3(0.0f, 0.0f, 1.0f),   // -y
                        glm::vec3(0.0f, -1.0f, 0.0f),  // +z
                        glm::vec3(0.0f, -1.0f, 0.0f)); // -z
        // clang-format on
    }

    return meshSizes;
}

MeshSizes GenerateCubeMeshTrisImpl(PrimitiveMesh3dParams& mesh, uint32_t subdivisions, glm::vec3 offset, glm::vec3 size)
{
    const MeshSizes meshSizes = GenerateCubeMeshVertices(mesh, subdivisions, offset, size);

    const uint32_t quadsPerSide = subdivisions + 1;
    const uint32_t quadsPerFace = quadsPerSide * quadsPerSide;
    const uint32_t indicesCount = quadsPerFace * 6 * 6;
    const uint32_t vertsPerSide = subdivisions + 2;
    const uint32_t vertsPerFace = vertsPerSide * vertsPerSide;

    if(mesh.indices.elementCount() >= indicesCount)
    {
        gpufmt::Writer writer(mesh.indices.format);

        if(!writer.isFormatWriteable()) { return {}; }

        for(uint32_t face = 0; face < 6; ++face)
        {
            uint32_t index = face * quadsPerFace * 6;

            for(uint32_t row = 0; row < quadsPerSide; ++row)
            {
                const uint32_t vertStart = face * vertsPerFace + row * vertsPerSide;
                const uint32_t nextRowVertStart = vertStart + vertsPerSide;

                for(uint32_t col = 0; col < quadsPerSide; ++col)
                {
                    // a----b
                    // | 1 /|
                    // |  / |
                    // | /  |
                    // |/ 2 |
                    // c----d

                    writer.writeTo(glm::u32vec4(col + vertStart), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + nextRowVertStart), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + vertStart + 1), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + vertStart + 1), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + nextRowVertStart), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + nextRowVertStart + 1), mesh.indices.accessElement(index++));
                }
            }
        }
    }

    return meshSizes;
}

MeshSizes
GenerateCubeMeshQuadsImpl(PrimitiveMesh3dParams& mesh, uint32_t subdivisions, glm::vec3 offset, glm::vec3 size)
{
    const MeshSizes meshSizes = GenerateCubeMeshVertices(mesh, subdivisions, offset, size);

    const uint32_t quadsPerSide = subdivisions + 1;
    const uint32_t quadsPerFace = quadsPerSide * quadsPerSide;
    const uint32_t indicesCount = quadsPerFace * 6 * 6;
    const uint32_t vertsPerSide = subdivisions + 2;
    const uint32_t vertsPerFace = vertsPerSide * vertsPerSide;

    if(mesh.indices.elementCount() >= indicesCount)
    {
        gpufmt::Writer writer(mesh.indices.format);

        if(!writer.isFormatWriteable()) { return {}; }

        for(uint32_t face = 0; face < 6; ++face)
        {
            uint32_t index = face * quadsPerFace * 4;

            for(uint32_t row = 0; row < quadsPerSide; ++row)
            {
                const uint32_t vertStart = face * vertsPerFace + row * vertsPerSide;
                const uint32_t nextRowVertStart = vertStart + vertsPerSide;

                for(uint32_t col = 0; col < quadsPerSide; ++col)
                {
                    writer.writeTo(glm::u32vec4(col + vertStart), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + nextRowVertStart), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + nextRowVertStart + 1), mesh.indices.accessElement(index++));
                    writer.writeTo(glm::u32vec4(col + vertStart + 1), mesh.indices.accessElement(index++));
                }
            }
        }
    }

    return meshSizes;
}

MeshSizes GenerateCubeMeshTris(PrimitiveMesh3dParams& mesh, uint32_t subdivisions, glm::vec3 offset, glm::vec3 size)
{
    return GenerateCubeMeshTrisImpl(mesh, subdivisions, offset, size);
}

MeshSizes GenerateCubeMeshQuads(PrimitiveMesh3dParams& mesh, uint32_t subdivisions, glm::vec3 offset, glm::vec3 size)
{
    return GenerateCubeMeshQuadsImpl(mesh, subdivisions, offset, size);
}

MeshSizes GenerateCubeMesh(CubeMeshTopologyType topologyType,
                           PrimitiveMesh3dParams& mesh,
                           uint32_t subdivisions,
                           glm::vec3 offset,
                           glm::vec3 size)
{
    const MeshSizes meshSizes = CalculateCubeMeshSizes(topologyType, subdivisions);

    if(mesh.positions.buffer.size() < meshSizes.vertexCount) { return {0, 0}; }

    switch(topologyType)
    {
    case scrap::CubeMeshTopologyType::Triangle: return GenerateCubeMeshTris(mesh, subdivisions, offset, size);
    case scrap::CubeMeshTopologyType::Quad: return GenerateCubeMeshQuads(mesh, subdivisions, offset, size);
    default: return {};
    }
}

CpuMesh GenerateCubeMesh(CubeMeshTopologyType topologyType, uint32_t subdivisions, glm::vec3 offset, glm::vec3 size)
{
    MeshSizes meshSizes = CalculateCubeMeshSizes(topologyType, subdivisions);

    CpuMesh cubeMesh(GetCubeMeshPrimitiveTopology(topologyType));
    cubeMesh.initIndices(IndexBufferFormat::UInt16, meshSizes.indexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Position, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Normal, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Tangent, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Binormal, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::TexCoord, 0, gpufmt::Format::R32G32_SFLOAT,
                                 meshSizes.vertexCount);

    PrimitiveMesh3dParams primitiveParams(cubeMesh);
    GenerateCubeMeshTris(primitiveParams, subdivisions, offset, size);

    return cubeMesh;
}
} // namespace scrap