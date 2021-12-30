#pragma once

#include "RenderDefs.h"
#include "d3d12/D3D12Fwd.h"

#include <memory>

namespace scrap::d3d12
{
struct VertexBuffer
{
    ShaderVertexSemantic semantic = ShaderVertexSemantic::Unknown;
    uint32_t semanticIndex = 0u;
    std::shared_ptr<d3d12::Buffer> buffer;
};
} // namespace scrap::d3d12