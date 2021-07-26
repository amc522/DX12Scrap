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

cbuffer VertexIndices : register(b0, space1)
{
    uint gVertexPositionIndex;
    uint gVertexTexCoordIndex;
    uint gVertexColorIndex;
}

SamplerState gSampler : register(s0);

VertexOutput VSMain(VertexInput input)
{
    Buffer<float2> positions = ResourceDescriptorHeap[gVertexPositionIndex];
    Buffer<float2> texCoords = ResourceDescriptorHeap[gVertexTexCoordIndex];
    Buffer<float4> colors = ResourceDescriptorHeap[gVertexColorIndex];

    VertexOutput output;

    output.clipPos = float4(positions[input.vertexId], 0.0, 1.0);
    output.uv = texCoords[input.vertexId];
    output.color = colors[input.vertexId];

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    Texture2D<float> gTexture = ResourceDescriptorHeap[4];

    return lerp(input.color, gTexture.Sample(gSampler, input.uv), 0.5);
}