#include "common.h"

struct VertexInput
{
    uint vertexId : SV_VertexID;
};

struct VertexOutput
{
    float4 clipPos : SV_Position;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

cbuffer VertexIndices : register(VERTEX_INDICES_CBUFFER_REGISTER)
{
    DECLARE_VERTEX_BUFFER(float2, Positions)
    DECLARE_VERTEX_BUFFER(float2, TexCoords)
    DECLARE_VERTEX_BUFFER(float4, Colors)
}

cbuffer ResourceIndices : register(RESOURCE_INDICES_CBUFFER_REGISTER)
{
    DECLARE_RESOURCE(Texture2D, float4, Texture);
}

SamplerState gSampler : register(s0);

VertexOutput VSMain(VertexInput input)
{
    Buffer<float2> positions = GetVertexPositions();
    Buffer<float2> texCoords = GetVertexTexCoords();
    Buffer<float4> colors = GetVertexColors();

    VertexOutput output;

    output.clipPos = float4(positions[input.vertexId], 0.0, 1.0);
    output.uv = texCoords[input.vertexId];

    float timeIntegerComponent;
    const float timeFrac = modf(gTime, timeIntegerComponent);
    const int timeSeconds = timeIntegerComponent;
    output.color = colors[input.vertexId] * ((timeSeconds % 2 == 0) ? timeFrac : 1.0 - timeFrac);

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    Texture2D<float4> gTexture = GetTexture();

    return lerp(input.color, gTexture.Sample(gSampler, input.uv), 0.5);
}