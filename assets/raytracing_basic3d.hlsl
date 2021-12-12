struct RayGenConstantBuffer
{
    float4x4 clipToWorld;
    float3 cameraWorldPos;
    float padding;
};

RaytracingAccelerationStructure Scene : register(t4, space1);
RWTexture2D<float4> RenderTarget : register(u3, space1);
ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b5, space1);

typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
};

inline void generateCameraRay(out float3 origin, out float3 direction)
{
    float2 clipPos = ((DispatchRaysIndex().xy + 0.5f) / DispatchRaysDimensions().xy) * 2.0 - 1.0;
    clipPos.y = -clipPos.y;

    float4 worldPos = mul(float4(clipPos, 0, 1), g_rayGenCB.clipToWorld);
    worldPos.xyz /= worldPos.w;

    origin = g_rayGenCB.cameraWorldPos.xyz;
    direction = normalize(worldPos.xyz - origin);
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
    float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
    payload.color = float4(RayTCurrent()/10.0, RayTCurrent()/10.0, RayTCurrent()/10.0, 1);
}

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    payload.color = float4(0, 0, 0, 1);
}