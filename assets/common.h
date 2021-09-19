#pragma pack_matrix(row_major)

#define BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name) g##ResourceObjectType##_##ResourceValueType##_##Name##DescriptorIndex

#define DECLARE_RESOURCE(ResourceObjectType, ResourceValueType, Name) \
uint BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name); \
ResourceObjectType<ResourceValueType> get##Name() { return ResourceDescriptorHeap[BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name)]; }

#define DECLARE_VERTEX_BUFFER(ValueType, Name) DECLARE_RESOURCE(Buffer, ValueType, Name)

#define VERTEX_INDICES_CBUFFER_REGISTER b0, space1
#define RESOURCE_INDICES_CBUFFER_REGISTER b2, space1


struct Frame
{
    float4x4 worldToView;
    float4x4 viewToClip;
    float4x4 worldToClip;
    float time;
    float frameTimeDelta;
};
ConstantBuffer<Frame> gFrame : register(b1, space1);