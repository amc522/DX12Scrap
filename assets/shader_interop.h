#ifdef __cplusplus

#    define SHADER_REGISTER(name, registerIndex, spaceIndex) constexpr RegisterSlot k##name{registerIndex, spaceIndex};
#    define SHADER_BUFFER_REGISTER(name, registerIndex, spaceIndex) SHADER_REGISTER(name, registerIndex, spaceIndex)
#    define SHADER_TEXTURE_REGISTER(name, registerIndex, spaceIndex) SHADER_REGISTER(name, registerIndex, spaceIndex)
#    define SHADER_UAV_REGISTER(name, registerIndex, spaceIndex) SHADER_REGISTER(name, registerIndex, spaceIndex)

#else

#    define SHADER_BUFFER_REGISTER(name, registerIndex, spaceIndex) b##registerIndex, space##spaceIndex
#    define SHADER_TEXTURE_REGISTER(name, registerIndex, spaceIndex) t##registerIndex, space##spaceIndex
#    define SHADER_UAV_REGISTER(name, registerIndex, spaceIndex) u##registerIndex, space##spaceIndex

#endif //__cplusplus

#define VERTEX_CBUFFER_REGISTER SHADER_BUFFER_REGISTER(VertexCBuffer, 0, 1)
#define FRAME_CBUFFER_REGISTER SHADER_BUFFER_REGISTER(FrameCBuffer, 1, 1)
#define RESOURCE_CBUFFER_REGISTER SHADER_BUFFER_REGISTER(ResourceCBuffer, 2, 1)
#define OUTPUT_BUFFER_REGISTER SHADER_UAV_REGISTER(OutputBuffer, 3, 1)
#define ACCELERATION_STRUCTURE_REGISTER SHADER_TEXTURE_REGISTER(AccelerationStructure, 4, 1)
#define OBJECT_CBUFFER_REGISTER SHADER_BUFFER_REGISTER(ObjectCBuffer, 5, 1)

#ifdef __cplusplus
namespace scrap::d3d12::shader
{
struct RegisterSlot
{
    unsigned int shaderRegister;
    unsigned int registerSpace;
};

VERTEX_CBUFFER_REGISTER
FRAME_CBUFFER_REGISTER
RESOURCE_CBUFFER_REGISTER
OUTPUT_BUFFER_REGISTER
ACCELERATION_STRUCTURE_REGISTER
OBJECT_CBUFFER_REGISTER
} // namespace scrap::d3d12::shader
#endif // __cplusplus