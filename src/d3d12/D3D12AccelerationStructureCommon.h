#pragma once

#include "Utility.h"

#include <string>

enum D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS;

namespace scrap::d3d12
{
enum class AccelerationStructureState
{
    Invalid,
    Building,
    Failed,
    Built
};

enum class AccelerationStructureFlags
{
    None = 0,
    AllowUpdates = 1 << 0,
    AllowCompaction = 1 << 1,
    MinimizeMemory = 1 << 2,
};
DEFINE_ENUM_BITWISE_OPERATORS(AccelerationStructureFlags)

enum class AccelerationStructureBuildOption
{
    FastTrace,
    FastBuild
};

D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS
TranslateAccelerationStructureBuildFlags(AccelerationStructureFlags flags,
                                         AccelerationStructureBuildOption buildOption);

enum class RaytracingGeometryFlags
{
    None,
    Transparent = 1 << 0,
    NoDuplicateAnyHit = 1 << 1,
};
DEFINE_ENUM_BITWISE_OPERATORS(RaytracingGeometryFlags)

struct AccelerationStructureParams
{
    AccelerationStructureFlags flags = AccelerationStructureFlags::None;
    AccelerationStructureBuildOption buildOption = AccelerationStructureBuildOption::FastTrace;
    size_t initialReservedObjects = 0;
    std::string name;
};
} // namespace scrap::d3d12