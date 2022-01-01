#pragma once

#include "EnumIterator.h"
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
[[nodiscard]] constexpr std::string_view ToStringView(GpuPreference gpuPreference)
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
    First = 0,
    Last = Count - 1,
    None,
};

template<>
[[nodiscard]] constexpr std::string_view ToStringView(GraphicsShaderStage shaderStage)
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
    Vertex = 1 << (size_t)GraphicsShaderStage::Vertex,
    Hull = 1 << (size_t)GraphicsShaderStage::Hull,
    Domain = 1 << (size_t)GraphicsShaderStage::Domain,
    Geometry = 1 << (size_t)GraphicsShaderStage::Geometry,
    Pixel = 1 << (size_t)GraphicsShaderStage::Pixel,
    VsGs = Vertex | Geometry,
    VsHsDs = Vertex | Hull | Domain,
    VsHsDsGs = Vertex | Hull | Domain | Geometry,
    VsPs = Vertex | Pixel,
    VsHsDsPs = Vertex | Hull | Domain | Pixel,
    VsGsPs = Vertex | Geometry | Pixel,
    VsHsDsGsPs = Vertex | Hull | Domain | Geometry | Pixel
};
DEFINE_ENUM_BITWISE_OPERATORS(GraphicsShaderStageMask);

[[nodiscard]] constexpr GraphicsShaderStageMask GraphicsShaderStageToMask(GraphicsShaderStage stage)
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
    First = 0,
    Last = Count - 1,
    None,
};

[[nodiscard]] constexpr std::string_view ToStringView(RaytracingShaderStage stage)
{
    switch(stage)
    {
    case scrap::RaytracingShaderStage::RayGen: return "RayGen";
    case scrap::RaytracingShaderStage::Intersection: return "Intersection";
    case scrap::RaytracingShaderStage::AnyHit: return "AnyHit";
    case scrap::RaytracingShaderStage::ClosestHit: return "ClosestHit";
    case scrap::RaytracingShaderStage::Miss: return "Miss";
    case scrap::RaytracingShaderStage::None: return "None";
    default: return "Unknown RaytracingShaderStage";
    }
}

enum class RaytracingShaderStageMask
{
    None = 0,
    RayGen = 1 << (uint32_t)RaytracingShaderStage::RayGen,
    Intersection = 1 << (uint32_t)RaytracingShaderStage::Intersection,
    AnyHit = 1 << (uint32_t)RaytracingShaderStage::AnyHit,
    ClosestHit = 1 << (uint32_t)RaytracingShaderStage::ClosestHit,
    Miss = 1 << (uint32_t)RaytracingShaderStage::Miss
};
DEFINE_ENUM_BITWISE_OPERATORS(RaytracingShaderStageMask);

[[nodiscard]] constexpr RaytracingShaderStageMask RaytracingShaderStageToMask(RaytracingShaderStage stage)
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

using RaytracingShaderStageEnumerator = MaskPairEnumerator<RaytracingShaderStageMask, RaytracingShaderStage>;

enum class RaytracingShaderType
{
    FixedStage,
    Callable,
    Unknown,
};

enum class RaytracingPipelineStage
{
    RayGen,
    HitGroup,
    Miss,
    Count,
    First = 0,
    Last = Count - 1,
    None,
};

[[nodiscard]] constexpr std::string_view ToStringView(RaytracingPipelineStage stage)
{
    switch(stage)
    {
    case RaytracingPipelineStage::RayGen: return "RayGen";
    case RaytracingPipelineStage::HitGroup: return "HitGroup";
    case RaytracingPipelineStage::Miss: return "Miss";
    default: return "Unknown RaytracingPipelineStage";
    }
}

[[nodiscard]] constexpr RaytracingPipelineStage RaytracingShaderStageToPipelineStage(RaytracingShaderStage stage)
{
    switch(stage)
    {
    case RaytracingShaderStage::RayGen: return RaytracingPipelineStage::RayGen;
    case RaytracingShaderStage::Intersection: return RaytracingPipelineStage::HitGroup;
    case RaytracingShaderStage::AnyHit: return RaytracingPipelineStage::HitGroup;
    case RaytracingShaderStage::ClosestHit: return RaytracingPipelineStage::HitGroup;
    case RaytracingShaderStage::Miss: return RaytracingPipelineStage::Miss;
    default: return RaytracingPipelineStage::None;
    }
}

enum class RaytracingPipelineStageMask
{
    None = 0,
    RayGen = 1 << ToUnderlying(RaytracingPipelineStage::RayGen),
    HitGroup = 1 << ToUnderlying(RaytracingPipelineStage::HitGroup),
    Miss = 1 << ToUnderlying(RaytracingPipelineStage::Miss)
};
DEFINE_ENUM_BITWISE_OPERATORS(RaytracingPipelineStageMask)

[[nodiscard]] constexpr RaytracingPipelineStageMask RaytracingPipelineStageToMask(RaytracingPipelineStage stage)
{
    switch(stage)
    {
    case RaytracingPipelineStage::RayGen: return RaytracingPipelineStageMask::RayGen;
    case RaytracingPipelineStage::HitGroup: return RaytracingPipelineStageMask::HitGroup;
    case RaytracingPipelineStage::Miss: return RaytracingPipelineStageMask::Miss;
    default: return RaytracingPipelineStageMask::None;
    }
}

[[nodiscard]] constexpr RaytracingPipelineStageMask
RaytracingShaderStageMaskToPipelineMask(RaytracingShaderStageMask shaderMask)
{
    RaytracingPipelineStageMask pipelineMask = RaytracingPipelineStageMask::None;
    if((shaderMask & RaytracingShaderStageMask::RayGen) != RaytracingShaderStageMask::None)
    {
        pipelineMask |= RaytracingPipelineStageMask::RayGen;
    }
    if((shaderMask & RaytracingShaderStageMask::AnyHit) != RaytracingShaderStageMask::None)
    {
        pipelineMask |= RaytracingPipelineStageMask::HitGroup;
    }
    if((shaderMask & RaytracingShaderStageMask::ClosestHit) != RaytracingShaderStageMask::None)
    {
        pipelineMask |= RaytracingPipelineStageMask::HitGroup;
    }
    if((shaderMask & RaytracingShaderStageMask::Intersection) != RaytracingShaderStageMask::None)
    {
        pipelineMask |= RaytracingPipelineStageMask::HitGroup;
    }
    if((shaderMask & RaytracingShaderStageMask::Miss) != RaytracingShaderStageMask::None)
    {
        pipelineMask |= RaytracingPipelineStageMask::Miss;
    }

    return pipelineMask;
}

using RaytracingPipelineStageEnumerator = MaskPairEnumerator<RaytracingPipelineStageMask, RaytracingPipelineStage>;

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
[[nodiscard]] constexpr std::string_view ToStringView(ShaderVertexSemantic semantic)
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
    Int16,
    UInt16,
    Int,
    UInt,
    Int64,
    UInt64,
    Half,
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

[[nodiscard]] constexpr size_t IndexBufferFormatByteSize(IndexBufferFormat format)
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

[[nodiscard]] constexpr gpufmt::Format TranslateIndexBufferFormat(IndexBufferFormat format)
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
