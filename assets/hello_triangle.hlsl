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

Buffer<float2> gVertexPositions : register(t1);
Buffer<float2> gVertexTexCoords : register(t2);
Buffer<float4> gVertexColors : register(t3);
Texture2D<float> gTexture : register(t4);
SamplerState gSampler : register(s0);

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;

    output.clipPos = float4(gVertexPositions[input.vertexId], 0.0, 1.0);
    output.uv = gVertexTexCoords[input.vertexId];
    output.color = gVertexColors[input.vertexId];

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    return lerp(input.color, gTexture.Sample(gSampler, input.uv), 0.5);
}