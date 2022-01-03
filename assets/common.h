#pragma pack_matrix(row_major)

// example bindless resource index variable name:
//   gTexture2D$float4$MyTextureDescriptorIndex
#define BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name) g##ResourceObjectType##$##ResourceValueType##$##Name##DescriptorIndex

#define DECLARE_RESOURCE(ResourceObjectType, ResourceValueType, Name) \
uint BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name); \
ResourceObjectType<ResourceValueType> get##Name() { return ResourceDescriptorHeap[BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name)]; }

// example bindless vertex buffer index variable name
//   gBuffer$float3$MyVertexBufferDescriptorIndex
#define DECLARE_VERTEX_BUFFER(ValueType, Name) DECLARE_RESOURCE(Buffer, ValueType, Name)

#define VERTEX_INDICES_CBUFFER_REGISTER b0, space1
#define RESOURCE_INDICES_CBUFFER_REGISTER b2, space1


struct Frame
{
    float4x4 worldToView;
    float4x4 viewToWorld;
    float4x4 viewToClip;
    float4x4 clipToView;
    float4x4 worldToClip;
    float4x4 clipToWorld;
    float3 cameraWorldPos;
    float time;
    float frameTimeDelta;
    float3 padding;
};
ConstantBuffer<Frame> gFrame : register(b1, space1);

struct ObjectConstantBuffer
{
    float4x4 objectToWorld;
    float4x4 worldToObject;
    float4x4 objectToView;
    float4x4 viewToObject;
    float4x4 objectToClip;
    float4x4 clipToObject;
};
ConstantBuffer<ObjectConstantBuffer> gObjectCb : register(b5, space1);