#define BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name) g##ResourceObjectType##_##ResourceValueType##_##Name##DescriptorIndex

#define DECLARE_RESOURCE(ResourceObjectType, ResourceValueType, Name) \
uint BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name); \
ResourceObjectType<ResourceValueType> Get##Name() { return ResourceDescriptorHeap[BINDLESS_RESOURCE_INDEX_VARIABLE_NAME(ResourceObjectType, ResourceValueType, Name)]; }

#define DECLARE_VERTEX_BUFFER(ValueType, Name) DECLARE_RESOURCE(Buffer, ValueType, Vertex##Name)

#define VERTEX_INDICES_CBUFFER_REGISTER b0, space1
#define RESOURCE_INDICES_CBUFFER_REGISTER b2, space1

cbuffer Frame : register(b1, space1)
{
    float gTime;
}