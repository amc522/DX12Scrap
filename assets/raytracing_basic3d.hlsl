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

RaytracingAccelerationStructure Scene : register(t4, space1);
RWTexture2D<float4> RenderTarget : register(u3, space1);
ConstantBuffer<VertexIndices> gVertexBuffers : register(VERTEX_INDICES_CBUFFER_REGISTER);
ConstantBuffer<ResourceIndices> gResources : register(RESOURCE_INDICES_CBUFFER_REGISTER);

SamplerState gSampler : register(s0);

typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
};

inline void generateCameraRay(out float3 origin, out float3 direction)
{
    float2 clipPos = ((DispatchRaysIndex().xy + 0.5f) / DispatchRaysDimensions().xy) * 2.0 - 1.0;
    clipPos.y = -clipPos.y;

    float4 worldPos = mul(gFrame.clipToWorld, float4(clipPos, 0, 1));
    worldPos.xyz /= worldPos.w;

    origin = gFrame.cameraWorldPos.xyz;
    direction = normalize(worldPos.xyz - origin);
}

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

[shader("raygeneration")]
void MyRaygenShader()
{
    float3 rayOrigin, rayDir;
    generateCameraRay(rayOrigin, rayDir);
    
    // Trace the ray.
    // Set the ray's extents.
    RayDesc ray;
    ray.Origin = rayOrigin;
    ray.Direction = rayDir;
    // Set TMin to a non-zero small value to avoid aliasing issues due to floating - point errors.
    // TMin should be kept small to prevent missing geometry at close contact areas.
    ray.TMin = 0.1;
    ray.TMax = 10000.0;
    RayPayload payload = { float4(0, 0, 0, 0) };
    TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);

    // Write the raytraced color to the output texture.
    RenderTarget[DispatchRaysIndex().xy] = payload.color;
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

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    payload.color = float4(0.2f, 0.0f, 0.4f, 1.0f);
}