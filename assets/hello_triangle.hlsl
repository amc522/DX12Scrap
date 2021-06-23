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

Texture2D<float> gTexture : register(t0);
SamplerState gSampler : register(s0);

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;

    switch(input.vertexId)
    {
    case 0:
        output.clipPos = float4(-0.5, -0.5, 0.0, 1.0);
        output.uv = float2(0.0, 1.0);
        output.color = float4(1.0, 0.0, 0.0, 1.0);
        break;
    case 1:
        output.clipPos = float4(0.0, 0.5, 0.0, 1.0);
        output.uv = float2(0.5, 0.0);
        output.color = float4(0.0, 1.0, 0.0, 1.0);
        break;
    case 2:
        output.clipPos = float4(0.5, -0.5, 0.0, 1.0);
        output.uv = float2(1.0, 1.0);
        output.color = float4(0.0, 0.0, 1.0, 1.0);
        break;
    default:
        output.clipPos = 0.0.xxxx;
        output.uv = 0.0.xx;
        output.color = float4(0.0.xxx, 1.0);
        break;
    }

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    return lerp(input.color, gTexture.Sample(gSampler, input.uv), 0.5);
}