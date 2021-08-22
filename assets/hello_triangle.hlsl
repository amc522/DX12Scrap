#define DECLARE_RESOURCE(ResourceObjectType, ResourceValueType, Name) \
uint g##Name##DescriptorIndex; \
ResourceObjectType<ResourceValueType> Get##Name() { return ResourceDescriptorHeap[g##Name##DescriptorIndex]; }

#define DECLARE_VERTEX_BUFFER(ValueType, Name) DECLARE_RESOURCE(Buffer, ValueType, Vertex##Name)

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
    DECLARE_VERTEX_BUFFER(float2, Positions)
    DECLARE_VERTEX_BUFFER(float2, TexCoords)
    DECLARE_VERTEX_BUFFER(float4, Colors)
}

cbuffer ResourceIndices : register(b1, space1)
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
    output.color = colors[input.vertexId];

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    Texture2D<float4> gTexture = GetTexture();

    return lerp(input.color, gTexture.Sample(gSampler, input.uv), 0.5);
}