struct VertexInput
{
    uint vertexId : SV_VertexID;
};

struct VertexOutput
{
    float4 clipPos : SV_Position;
    float4 color : COLOR;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;

    switch(input.vertexId)
    {
    case 0:
        output.clipPos = float4(-0.5, -0.5, 0.0, 1.0);
        output.color = float4(1.0, 0.0, 0.0, 1.0);
        break;
    case 1:
        output.clipPos = float4(0.0, 0.5, 0.0, 1.0);
        output.color = float4(0.0, 1.0, 0.0, 1.0);
        break;
    case 2:
        output.clipPos = float4(0.5, -0.5, 0.0, 1.0);
        output.color = float4(0.0, 0.0, 1.0, 1.0);
        break;
    default:
        output.clipPos = 0.0.xxxx;
        output.color = float4(0.0.xxx, 1.0);
        break;
    }

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    return input.color;
}