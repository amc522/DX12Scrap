#pragma once

#include "RenderDefs.h"

#include <cputex/definitions.h>
#include <d3d12.h>
#include <d3dcommon.h>

namespace scrap::d3d12
{
[[nodiscard]] constexpr D3D_PRIMITIVE_TOPOLOGY TranslatePrimitiveTopology(PrimitiveTopology topology)
{
    switch(topology)
    {
    case scrap::PrimitiveTopology::Undefined: return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    case scrap::PrimitiveTopology::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case scrap::PrimitiveTopology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case scrap::PrimitiveTopology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case scrap::PrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case scrap::PrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case scrap::PrimitiveTopology::LineListAdj: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
    case scrap::PrimitiveTopology::LineStripAdj: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
    case scrap::PrimitiveTopology::TriangleListAdj: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
    case scrap::PrimitiveTopology::TriangleStripAdj: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
    case scrap::PrimitiveTopology::Patch_1ControlPoint: return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
    case scrap::PrimitiveTopology::Patch_2ControlPoint: return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
    case scrap::PrimitiveTopology::Patch_3ControlPoint: return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
    case scrap::PrimitiveTopology::Patch_4ControlPoint: return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
    case scrap::PrimitiveTopology::Patch_5ControlPoint: return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
    case scrap::PrimitiveTopology::Patch_6ControlPoint: return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
    default: assert(false); return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

[[nodiscard]] constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslatePrimitiveTopologyType(PrimitiveTopology topology)
{
    switch(topology)
    {
    case scrap::PrimitiveTopology::Undefined: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    case scrap::PrimitiveTopology::PointList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case scrap::PrimitiveTopology::LineList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case scrap::PrimitiveTopology::LineStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case scrap::PrimitiveTopology::TriangleList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case scrap::PrimitiveTopology::TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case scrap::PrimitiveTopology::LineListAdj: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case scrap::PrimitiveTopology::LineStripAdj: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case scrap::PrimitiveTopology::TriangleListAdj: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case scrap::PrimitiveTopology::TriangleStripAdj: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case scrap::PrimitiveTopology::Patch_1ControlPoint: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    case scrap::PrimitiveTopology::Patch_2ControlPoint: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    case scrap::PrimitiveTopology::Patch_3ControlPoint: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    case scrap::PrimitiveTopology::Patch_4ControlPoint: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    case scrap::PrimitiveTopology::Patch_5ControlPoint: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    case scrap::PrimitiveTopology::Patch_6ControlPoint: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default: assert(false); return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
}

[[nodiscard]] constexpr D3D12_RESOURCE_DIMENSION TranslateResourceDimension(cputex::TextureDimension dimension)
{
    switch(dimension)
    {
    case cputex::TextureDimension::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    case cputex::TextureDimension::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    case cputex::TextureDimension::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    case cputex::TextureDimension::TextureCube: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    default: return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }
}

[[nodiscard]] constexpr D3D12_SRV_DIMENSION TranslateSrvDimension(cputex::TextureDimension dimension, size_t arraySize)
{
    switch(dimension)
    {
    case cputex::TextureDimension::Texture1D:
        return (arraySize > 1) ? D3D12_SRV_DIMENSION_TEXTURE1DARRAY : D3D12_SRV_DIMENSION_TEXTURE1D;
    case cputex::TextureDimension::Texture2D:
        return (arraySize > 1) ? D3D12_SRV_DIMENSION_TEXTURE2DARRAY : D3D12_SRV_DIMENSION_TEXTURE2D;
    case cputex::TextureDimension::Texture3D: return D3D12_SRV_DIMENSION_TEXTURE3D;
    case cputex::TextureDimension::TextureCube:
        return (arraySize > 1) ? D3D12_SRV_DIMENSION_TEXTURECUBEARRAY : D3D12_SRV_DIMENSION_TEXTURECUBE;
    default: return D3D12_SRV_DIMENSION_UNKNOWN;
    }
}
} // namespace scrap::d3d12