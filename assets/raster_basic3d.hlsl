#include "common.h"

struct VertexInput
{
    uint vertexId : SV_VertexID;
};

struct VertexOutput
{
    float4 clipPos : SV_Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VertexIndices
{
    DECLARE_VERTEX_BUFFER(float3, Positions)
    DECLARE_VERTEX_BUFFER(float3, Normals)
    DECLARE_VERTEX_BUFFER(float2, TexCoords)
};
ConstantBuffer<VertexIndices> gVertexBuffers : register(VERTEX_CBUFFER_REGISTER);

struct ResourceIndices
{
    DECLARE_RESOURCE(Texture2D, float4, Texture);
};
ConstantBuffer<ResourceIndices> gResources : register(RESOURCE_CBUFFER_REGISTER);

SamplerState gSampler : register(s0);

VertexOutput VSMain(VertexInput input)
{
    Buffer<float3> positions = gVertexBuffers.getPositions();
    Buffer<float3> normals = gVertexBuffers.getNormals();
    Buffer<float2> texCoords = gVertexBuffers.getTexCoords();

    VertexOutput output;

    output.clipPos = mul(gObjectCb.objectToClip, float4(positions[input.vertexId], 1.0));
    output.worldNormal = normals[input.vertexId];
    output.uv = texCoords[input.vertexId];

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    Texture2D<float4> texture = gResources.getTexture();

    return texture.Sample(gSampler, input.uv);
}