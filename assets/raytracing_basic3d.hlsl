#include "common.h"

struct VertexIndices
{
    DECLARE_VERTEX_BUFFER(float3, Position)
    DECLARE_VERTEX_BUFFER(float2, Uv)
};

struct ResourceIndices
{
    DECLARE_RESOURCE(Texture2D, float4, Texture)
    DECLARE_RESOURCE(Buffer, uint16_t, IndexBuffer)
};

ConstantBuffer<VertexIndices> gVertexBuffers : register(VERTEX_INDICES_CBUFFER_REGISTER);
ConstantBuffer<ResourceIndices> gResources : register(RESOURCE_INDICES_CBUFFER_REGISTER);

SamplerState gSampler : register(s0);

typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
};

inline float2 interpolateHit(in float2 vertexElements[3], BuiltInTriangleIntersectionAttributes intersectionAttr)
{
    return vertexElements[0] +
        intersectionAttr.barycentrics.x * (vertexElements[1] - vertexElements[0]) +
        intersectionAttr.barycentrics.y * (vertexElements[2] - vertexElements[0]);
}

inline float3 interpolateHit(in float3 vertexElements[3], BuiltInTriangleIntersectionAttributes intersectionAttr)
{
    return vertexElements[0] +
        intersectionAttr.barycentrics.x * (vertexElements[1] - vertexElements[0]) +
        intersectionAttr.barycentrics.y * (vertexElements[2] - vertexElements[0]);
}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{
    const uint startVertexIndex = PrimitiveIndex() * 3;
    Buffer<uint16_t> indexBuffer = gResources.getIndexBuffer();
    Buffer<float3> positions = gVertexBuffers.getPosition();
    Buffer<float2> uvs = gVertexBuffers.getUv();

    float2 vertexUvs[3];
    for(uint i = 0; i < 3; ++i)
    {
        vertexUvs[i] = uvs[indexBuffer[startVertexIndex + i]];
    }

    const float2 uv = interpolateHit(vertexUvs, attr);

    Texture2D<float4> texture = gResources.getTexture();
    float4 color = texture.SampleGrad(gSampler, uv, 0.0, 0.0);
    payload.color = float4(color.rgb, 1.0);
}