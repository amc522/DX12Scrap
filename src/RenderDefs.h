#pragma once

#include "StringUtils.h"
#include "Utility.h"

#include <string>
#include <string_view>
#include <vector>

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
    case scrap::GraphicsShaderStage::Vertex:
        return "Vertex";
    case scrap::GraphicsShaderStage::Hull:
        return "Hull";
    case scrap::GraphicsShaderStage::Domain:
        return "Domain";
    case scrap::GraphicsShaderStage::Geometry:
        return "Geometry";
    case scrap::GraphicsShaderStage::Pixel:
        return "Pixel";
    default:
        return "Unknow GraphicsShaderstage";
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

inline GraphicsShaderStageMask GraphicsShaderStageToMask(GraphicsShaderStage stage)
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

enum class ResourceAccessFlags
{
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
    TexCoord,
    Color,
    Count,
    Unknown
};

struct ShaderVertexElement
{
    ShaderVertexSemantic semantic = ShaderVertexSemantic::Unknown;
    uint32_t semanticIndex = 0;
    uint32_t index = 0;
};

struct ShaderResource
{
    std::string name;
    uint64_t nameHash = 0;
    uint32_t index = 0;
};

struct ShaderInputs
{
    std::vector<ShaderVertexElement> vertexElements;
    std::vector<ShaderResource> resources;
};
} // namespace scrap

template<>
struct fmt::formatter<scrap::GpuPreference> : public scrap::ToStringViewFormatter<scrap::GpuPreference>
{};

template<>
struct fmt::formatter<scrap::GraphicsShaderStage> : public scrap::ToStringViewFormatter<scrap::GraphicsShaderStage>
{};