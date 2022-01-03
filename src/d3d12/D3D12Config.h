#pragma once

#include <cstdint>
#include "../assets/shader_interop.h"

namespace scrap::d3d12
{
constexpr size_t kFrameBufferCount = 2u;

// Bindless resources
constexpr uint32_t kMaxBindlessVertexBuffers = 4;
constexpr uint32_t kMaxBindlessResources = 8;

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