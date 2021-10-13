#pragma once

#include "StringUtils.h"
#include "Utility.h"

#include <string>
#include <string_view>
#include <vector>

#include <gpufmt/format.h>

namespace scrap
{
enum class GpuPreference
{
    None,
    HighPerformance,
    MinimumPower,
    MaximumMemory,
    MinimumMemory
};

template<>
constexpr std::string_view ToStringView(GpuPreference gpuPreference)
{
    switch(gpuPreference)
    {
    case scrap::GpuPreference::None: return "None";
    case scrap::GpuPreference::HighPerformance: return "HighPerformance";
    case scrap::GpuPreference::MinimumPower: return "MinimumPower";
    case scrap::GpuPreference::MaximumMemory: return "MaximumMemory";
    case scrap::GpuPreference::MinimumMemory: return "MinimumMemory";
    default: return "Unknown GpuPreference";
    }
}

enum class GraphicsShaderStage
{
    Vertex = 0,
    Hull,
    Domain,
    Geometry,
    Pixel,
    Count,
    First = Vertex,
    Last = Pixel,
    None,
};

template<>
constexpr std::string_view ToStringView(GraphicsShaderStage shaderStage)
{
    switch(shaderStage)
    {
    case scrap::GraphicsShaderStage::Vertex: return "Vertex";
    case scrap::GraphicsShaderStage::Hull: return "Hull";
    case scrap::GraphicsShaderStage::Domain: return "Domain";
    case scrap::GraphicsShaderStage::Geometry: return "Geometry";
    case scrap::GraphicsShaderStage::Pixel: return "Pixel";
    default: return "Unknow GraphicsShaderstage";
    }
}

enum class GraphicsShaderStageMask
{
    None = 0,
    Vertex = 1 << 0,
    Hull = 1 << 1,
    Domain = 1 << 2,
    Geometry = 1 << 3,
    Pixel = 1 << 4,
    VsGs = Vertex | Geometry,
    VsHsDs = Vertex | Hull | Domain,
    VsHsDsGs = Vertex | Hull | Domain | Geometry,
    VsPs = Vertex | Pixel,
    VsHsDsPs = Vertex | Hull | Domain | Pixel,
    VsGsPs = Vertex | Geometry | Pixel,
    VsHsDsGsPs = Vertex | Hull | Domain | Geometry | Pixel
};
DEFINE_ENUM_BITWISE_OPERATORS(GraphicsShaderStageMask);

constexpr GraphicsShaderStageMask GraphicsShaderStageToMask(GraphicsShaderStage stage)
{
    switch(stage)
    {
    case scrap::GraphicsShaderStage::Vertex: return GraphicsShaderStageMask::Vertex;
    case scrap::GraphicsShaderStage::Hull: return GraphicsShaderStageMask::Hull;
    case scrap::GraphicsShaderStage::Domain: return GraphicsShaderStageMask::Domain;
    case scrap::GraphicsShaderStage::Geometry: return GraphicsShaderStageMask::Geometry;
    case scrap::GraphicsShaderStage::Pixel: return GraphicsShaderStageMask::Pixel;
    default: return GraphicsShaderStageMask::None;
    }
}

enum class RaytracingShaderStage
{
    RayGen,
    Intersection,
    AnyHit,
    ClosestHit,
    Miss,
    Count,
    First = RayGen,
    Last = Miss,
    None,
};

constexpr std::string_view ToStringView(RaytracingShaderStage stage)
{
    switch(stage)
    {
    case scrap::RaytracingShaderStage::RayGen:
        return "RayGen";
    case scrap::RaytracingShaderStage::Intersection:
        return "Intersection";
    case scrap::RaytracingShaderStage::AnyHit:
        return "AnyHit";
    case scrap::RaytracingShaderStage::ClosestHit:
        return "ClosestHit";
    case scrap::RaytracingShaderStage::Miss:
        return "Miss";
    case scrap::RaytracingShaderStage::None:
        return "None";
    default:
        return "Unknown RaytracingShaderStage";
    }
}

enum class RaytracingShaderStageMask
{
    None = 0,
    RayGen = 1 << 0,
    Intersection = 1 << 1,
    AnyHit = 1 << 2,
    ClosestHit = 1 << 3,
    Miss = 1 << 4,
    RgChMs = RayGen | ClosestHit | Miss,
    RgIsAhChMs = RayGen | Intersection | AnyHit | ClosestHit | Miss,
};
DEFINE_ENUM_BITWISE_OPERATORS(RaytracingShaderStageMask);

constexpr RaytracingShaderStageMask RaytracingShaderStageToMask(RaytracingShaderStage stage)
{
    switch(stage)
    {
    case scrap::RaytracingShaderStage::RayGen: return RaytracingShaderStageMask::RayGen;
    case scrap::RaytracingShaderStage::Intersection: return RaytracingShaderStageMask::Intersection;
    case scrap::RaytracingShaderStage::AnyHit: return RaytracingShaderStageMask::AnyHit;
    case scrap::RaytracingShaderStage::ClosestHit: return RaytracingShaderStageMask::ClosestHit;
    case scrap::RaytracingShaderStage::Miss: return RaytracingShaderStageMask::Miss;
    default: return RaytracingShaderStageMask::None;
    }
}

enum class ResourceAccessFlags
{
    None = 0,
    CpuRead = 1 << 0,
    CpuWrite = 1 << 1,
    GpuRead = 1 << 2,
    GpuWrite = 1 << 3,
    GpuRW = GpuRead | GpuWrite,
};
DEFINE_ENUM_BITWISE_OPERATORS(ResourceAccessFlags);

enum class ShaderVertexSemantic
{
    Position,
    Normal,
    Tangent,
    Binormal,
    TexCoord,
    Color,
    Count,
    Unknown
};

template<>
constexpr std::string_view ToStringView(ShaderVertexSemantic semantic)
{
    switch(semantic)
    {
    case scrap::ShaderVertexSemantic::Position: return "Position";
    case scrap::ShaderVertexSemantic::Normal: return "Normal";
    case scrap::ShaderVertexSemantic::Tangent: return "Tangent";
    case scrap::ShaderVertexSemantic::Binormal: return "Binormal";
    case scrap::ShaderVertexSemantic::TexCoord: return "TexCoord";
    case scrap::ShaderVertexSemantic::Color: return "Color";
    case scrap::ShaderVertexSemantic::Unknown: return "Unknown";
    default: return "Unhandled ShaderVertexSemantic";
    }
}

enum class ShaderResourceType
{
    AppendBuffer,
    Buffer,
    ByteAddressBuffer,
    ConsumeBuffer,
    RwBuffer,
    RwByteAddressBuffer,
    RwStructuredBuffer,
    RwTexture,
    StructuredBuffer,
    Texture,
    Unknown
};

enum class ShaderResourceDimension
{
    Buffer,
    Texture1d,
    Texture1dArray,
    Texture2d,
    Texture2dArray,
    Texture3d,
    TextureCube,
    TextureCubeArray,
    Unknown
};

enum class ShaderResourceReturnType
{
    Unorm,
    Snorm,
    Int,
    UInt,
    Float,
    Double,
    None
};

struct ShaderResource
{
    std::string name;
    uint64_t nameHash = 0;
    uint32_t index = 0;
    ShaderResourceType type = ShaderResourceType::Unknown;
    ShaderResourceDimension dimension = ShaderResourceDimension::Unknown;
    ShaderResourceReturnType returnType = ShaderResourceReturnType::None;
    uint8_t returnTypeComponentCount = 1;
    bool multisample = false;
};

struct ShaderVertexElement : public ShaderResource
{
    ShaderVertexSemantic semantic = ShaderVertexSemantic::Unknown;
    uint32_t semanticIndex = uint32_t(-1);
};

struct ShaderInputs
{
    std::vector<ShaderVertexElement> vertexElements;
    std::vector<ShaderResource> resources;
};

enum class IndexBufferFormat
{
    UInt16,
    Int16,
    UInt32,
    Int32,
};

constexpr size_t IndexBufferFormatByteSize(IndexBufferFormat format)
{
    switch(format)
    {
    case IndexBufferFormat::UInt16: return sizeof(uint16_t);
    case IndexBufferFormat::Int16: return sizeof(int16_t);
    case IndexBufferFormat::UInt32: return sizeof(uint32_t);
    case IndexBufferFormat::Int32: return sizeof(int32_t);
    default: return 0;
    }
}

constexpr gpufmt::Format TranslateIndexBufferFormat(IndexBufferFormat format)
{
    switch(format)
    {
    case IndexBufferFormat::UInt16: return gpufmt::Format::R16_UINT;
    case IndexBufferFormat::Int16: return gpufmt::Format::R16_SINT;
    case IndexBufferFormat::UInt32: return gpufmt::Format::R32_UINT;
    case IndexBufferFormat::Int32: return gpufmt::Format::R32_SINT;
    default: return gpufmt::Format::UNDEFINED;
    }
}

enum class PrimitiveTopology
{
    Undefined,
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    LineListAdj,
    LineStripAdj,
    TriangleListAdj,
    TriangleStripAdj,
    Patch_1ControlPoint,
    Patch_2ControlPoint,
    Patch_3ControlPoint,
    Patch_4ControlPoint,
    Patch_5ControlPoint,
    Patch_6ControlPoint,
};
} // namespace scrap

template<>
struct fmt::formatter<scrap::GpuPreference> : public scrap::ToStringViewFormatter<scrap::GpuPreference>
{};

template<>
struct fmt::formatter<scrap::GraphicsShaderStage> : public scrap::ToStringViewFormatter<scrap::GraphicsShaderStage>
{};

template<>
struct fmt::formatter<scrap::ShaderVertexSemantic> : public scrap::ToStringViewFormatter<scrap::ShaderVertexSemantic>
{};