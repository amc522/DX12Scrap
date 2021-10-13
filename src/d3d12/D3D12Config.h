#pragma once

#include <cstdint>

namespace scrap::d3d12
{
constexpr size_t kFrameBufferCount = 2u;

// Bindless resources
constexpr uint32_t kMaxBindlessVertexBuffers = 4;
constexpr uint32_t kMaxBindlessResources = 8;

constexpr uint32_t kEngineShaderRegisterSpace = 1;

struct ShaderRegisterSlot
{
    uint32_t shaderRegister;
    uint32_t registerSpace = kEngineShaderRegisterSpace;
};

namespace reservedShaderRegister
{
constexpr ShaderRegisterSlot kVertexCB{0};
constexpr ShaderRegisterSlot kFrameCB{1};
constexpr ShaderRegisterSlot kResourceCB{2};
} // namespace reservedShaderRegister

namespace RasterRootParamSlot
{
enum Value
{
    FrameCB = 0,
    ResourceIndices,
    VertexIndices,
    Count
};
}
};

} // namespace scrap::d3d12