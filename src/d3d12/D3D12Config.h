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
constexpr ShaderRegisterSlot kOutputBuffer{3};
constexpr ShaderRegisterSlot kAccelerationStructure{4};
constexpr ShaderRegisterSlot kObjectCB{5};
} // namespace reservedShaderRegister

namespace RasterRootParamSlot
{
enum Value
{
    FrameCB = 0,
    ResourceIndices,
    VertexIndices,
    ObjectCB,
    Count
};
}

namespace RaytracingGlobalRootParamSlot
{
enum Value
{
    OutputView,
    AccelerationStructure,
    FrameCB,
    Count
};
}

namespace RaytracingRayGenLocalRootParamSlot
{
enum Value
{
    Count
};
}

namespace RaytracingAnyHitLocalRootParamSlot
{
enum Value
{
    Count
};
}

namespace RaytracingIntersectionLocalRootParamSlot
{
enum Value
{
    Count
};
}

namespace RaytracingClosestHitLocalRootParamSlot
{
enum Value
{
    ResourceIndices,
    VertexIndices,
    Count
};
}

namespace RaytracingMissLocalRootParamSlot
{
enum Value
{
    Count
};
}

} // namespace scrap::d3d12