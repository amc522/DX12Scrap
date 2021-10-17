#include "d3d12/D3D12AccelerationStructureCommon.h"

#include <d3d12.h>

namespace scrap::d3d12
{
D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS
TranslateAccelerationStructureBuildFlags(AccelerationStructureFlags flags, AccelerationStructureBuildOption buildOption)
{
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS d3d12Flags =
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

    switch(buildOption)
    {
    case AccelerationStructureBuildOption::FastBuild:
        d3d12Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
        break;
    case AccelerationStructureBuildOption::FastTrace:
        d3d12Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        break;
    }

    if((flags & AccelerationStructureFlags::AllowCompaction) == AccelerationStructureFlags::AllowCompaction)
    {
        d3d12Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
    }
    if((flags & AccelerationStructureFlags::AllowUpdates) == AccelerationStructureFlags::AllowUpdates)
    {
        d3d12Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
    }
    if((flags & AccelerationStructureFlags::MinimizeMemory) == AccelerationStructureFlags::MinimizeMemory)
    {
        d3d12Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
    }

    return d3d12Flags;
}
} // namespace scrap::d3d12