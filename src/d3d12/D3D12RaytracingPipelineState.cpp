#include "d3d12/D3D12RaytracingPipelineState.h"

#include "EnumIterator.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12RaytracingShader.h"

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
RaytracingPipelineState::RaytracingPipelineState(RaytracingPipelineStateParams&& params)
    : mHitGroupName(Utf8ToWideString(params.hitGroupName))
    , mPrimitiveType(params.primitiveType)
    , mMaxRecursionDepth(params.maxRecursionDepth)
{
    mShaders.assign(params.shaders.begin(), params.shaders.end());

    mFixedStageShaderParams[(size_t)RaytracingShaderStage::RayGen] = params.fixedStages.raygen;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::Intersection] = params.fixedStages.intersection;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::AnyHit] = params.fixedStages.anyHit;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::ClosestHit] = params.fixedStages.closestHit;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::Miss] = params.fixedStages.miss;

    RaytracingShaderStageMask shaderStageMask = RaytracingShaderStageMask::None;

    for(RaytracingShaderStage stage : Enumerator<RaytracingShaderStage>())
    {
        shaderStageMask = (mFixedStageShaderParams[(size_t)RaytracingShaderStage::RayGen].shaderIndex !=
                           RaytracingPipelineStateShaderParams::kInvalidIndex)
                              ? RaytracingShaderStageToMask(stage)
                              : RaytracingShaderStageMask::None;
    }

    mPipelineStages = RaytracingShaderStageMaskToPipelineMask(shaderStageMask);

    mValidFixedStageExportCount =
        (uint32_t)std::count_if(mFixedStageShaderParams.cbegin(), mFixedStageShaderParams.cend(),
                                [](const auto& shaderExport) { return shaderExport.hasValidShaderEntryPoint(); });

    mCallableShaderParams.assign(params.callables.begin(), params.callables.end());

    mLocalRootSignatures.reserve(params.localRootSignatures.size());
    for(ComPtr<ID3D12RootSignature>& localRootSignature : params.localRootSignatures)
    {
        mLocalRootSignatures.emplace_back(std::move(localRootSignature));
    }
}

template<class T>
using MonotonicVector = eastl::vector<T, MonotonicBufferEastlAllocator<T>>;

template<class T>
inline MonotonicVector<T> makeMonotonicVector(MonotonicBuffer& buffer)
{
    return MonotonicVector<T>{MonotonicBufferEastlAllocator<T>(buffer)};
}

void RaytracingPipelineState::create()
{
    if(mShaders.empty()) { return; }

    for(std::shared_ptr<RaytracingShader>& shader : mShaders)
    {
        if(shader->status() == RaytracingShaderState::Initialized) { shader->create(); }
    }

    for(std::shared_ptr<RaytracingShader>& shader : mShaders)
    {
        while(shader->status() == RaytracingShaderState::Compiling)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if(shader->status() == RaytracingShaderState::Failed) { return; }
    }

    struct LocalRootSignatureStructures
    {
        D3D12_LOCAL_ROOT_SIGNATURE signature;
        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION exportsAssociation;
    };

    const size_t subObjectCount = mShaders.size()                   // dxil libraries
                                  + 1                               // hit group
                                  + mLocalRootSignatures.size() * 2 // local root signatures and associations
                                  + 1;                              // pipeline config

    const size_t dxilLibraryDescCount = mShaders.size();
    const size_t exportDescsCount = mValidFixedStageExportCount + mCallableShaderParams.size();
    const size_t localRootSignatureStructureCount = mLocalRootSignatures.size();
    const size_t localRootSignatureExportsCount = mValidFixedStageExportCount + mCallableShaderParams.size();

    MemoryCalculator memoryCalc;
    memoryCalc.add<D3D12_STATE_SUBOBJECT>(subObjectCount);
    memoryCalc.add<D3D12_DXIL_LIBRARY_DESC>(dxilLibraryDescCount);
    memoryCalc.add<D3D12_EXPORT_DESC>(exportDescsCount);
    memoryCalc.add<D3D12_HIT_GROUP_DESC>(1);
    memoryCalc.add<LocalRootSignatureStructures>(localRootSignatureStructureCount);
    memoryCalc.add<LPCWSTR>(localRootSignatureExportsCount);
    memoryCalc.add<D3D12_RAYTRACING_PIPELINE_CONFIG1>(1);

    mMonotonicBuffer = MonotonicBuffer(memoryCalc.getByteSize());

    auto subObjects = makeMonotonicVector<D3D12_STATE_SUBOBJECT>(mMonotonicBuffer);
    subObjects.reserve(subObjectCount);

#pragma region dxil_libraries
    // DXIL LIBRARIES
    auto dxilLibDescs = makeMonotonicVector<D3D12_DXIL_LIBRARY_DESC>(mMonotonicBuffer);
    dxilLibDescs.reserve(dxilLibraryDescCount);
    auto exportDescs = makeMonotonicVector<D3D12_EXPORT_DESC>(mMonotonicBuffer);
    exportDescs.reserve(exportDescsCount);

    ptrdiff_t exportOffset = 0;

    for(size_t shaderIndex = 0; shaderIndex < mShaders.size(); ++shaderIndex)
    {
        const std::shared_ptr<RaytracingShader>& shader = mShaders[shaderIndex];
        std::span fixedStageShaders = shader->getFixedStageShaders();
        std::span callableShaders = shader->getCallableShaders();

        for(const RaytracingPipelineStateShaderParams& shaderParams : mFixedStageShaderParams)
        {
            if(!shaderParams.hasValidShaderEntryPoint() || shaderParams.shaderIndex != shaderIndex) { continue; }

            D3D12_EXPORT_DESC& desc = exportDescs.emplace_back();
            desc.Name = fixedStageShaders[shaderParams.shaderEntryPointIndex].wideEntryPoint.c_str();
        }

        for(const RaytracingPipelineStateShaderParams& shaderParams : mCallableShaderParams)
        {
            if(!shaderParams.hasValidShaderEntryPoint() || shaderParams.shaderIndex != shaderIndex) { continue; }

            D3D12_EXPORT_DESC& desc = exportDescs.emplace_back();
            desc.Name = callableShaders[shaderParams.shaderEntryPointIndex].wideEntryPoint.c_str();
        }

        D3D12_DXIL_LIBRARY_DESC& dxilLibDesc = dxilLibDescs.emplace_back();
        dxilLibDesc.DXILLibrary = shader->getShaderByteCode();
        dxilLibDesc.NumExports = uint32_t(exportDescs.size() - exportOffset);
        dxilLibDesc.pExports = exportDescs.data() + exportOffset;

        exportOffset += dxilLibDesc.NumExports;

        D3D12_STATE_SUBOBJECT& subObject = subObjects.emplace_back();
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        subObject.pDesc = &dxilLibDesc;
    }

#pragma endregion

#pragma region hit_group
    // HIT GROUP
    auto GetFixedStageEntryPointName = [&](RaytracingShaderStage stage) -> LPCWSTR {
        const RaytracingPipelineStateShaderParams shaderParams = mFixedStageShaderParams[(size_t)stage];
        if(!shaderParams.hasValidShaderEntryPoint()) { return nullptr; }

        std::span fixedStageShaders = mShaders[shaderParams.shaderIndex]->getFixedStageShaders();

        return fixedStageShaders[shaderParams.shaderEntryPointIndex].wideEntryPoint.c_str();
    };

    auto* hitGroupDesc = mMonotonicBuffer.allocate<D3D12_HIT_GROUP_DESC>(1);
    hitGroupDesc->IntersectionShaderImport = GetFixedStageEntryPointName(RaytracingShaderStage::Intersection);
    hitGroupDesc->AnyHitShaderImport = GetFixedStageEntryPointName(RaytracingShaderStage::AnyHit);
    hitGroupDesc->ClosestHitShaderImport = GetFixedStageEntryPointName(RaytracingShaderStage::ClosestHit);
    hitGroupDesc->HitGroupExport = mHitGroupName.c_str();
    hitGroupDesc->Type = (mPrimitiveType == RaytracingPipelineStatePrimitiveType::Triangles)
                             ? D3D12_HIT_GROUP_TYPE_TRIANGLES
                             : D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects.emplace_back();
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        subObject.pDesc = hitGroupDesc;
    }
#pragma endregion

#pragma region local_root_signatures
    // LOCAL ROOT SIGNATURES
    auto localRootSignatureStructures = makeMonotonicVector<LocalRootSignatureStructures>(mMonotonicBuffer);
    localRootSignatureStructures.reserve(localRootSignatureStructureCount);

    auto localRootSignatureExports = makeMonotonicVector<LPCWSTR>(mMonotonicBuffer);
    localRootSignatureExports.reserve(localRootSignatureExportsCount);
    size_t localRootSignatureExportsOffset = 0;

    for(size_t localRootSignatureIndex = 0; localRootSignatureIndex < mLocalRootSignatures.size();
        ++localRootSignatureIndex)
    {
        ID3D12RootSignature* localRootSignature = mLocalRootSignatures[localRootSignatureIndex].get();
        auto& structures = localRootSignatureStructures.emplace_back();
        structures.signature.pLocalRootSignature = localRootSignature;

        D3D12_STATE_SUBOBJECT& localRootSigSubObject = subObjects.emplace_back();
        localRootSigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
        localRootSigSubObject.pDesc = &structures.signature;

        structures.exportsAssociation.pSubobjectToAssociate = &localRootSigSubObject;
        structures.exportsAssociation.NumExports = 0;

        auto UsesCurrentLocalRootSignature =
            [localRootSignatureIndex](const RaytracingPipelineStateShaderParams& shaderParams) {
                return shaderParams.localRootSignatureIndex == localRootSignatureIndex;
            };

        for(const RaytracingPipelineStateShaderParams& shaderParams : mFixedStageShaderParams)
        {
            if(shaderParams.localRootSignatureIndex == localRootSignatureIndex)
            {
                ++structures.exportsAssociation.NumExports;
                localRootSignatureExports.push_back(getShaderEntryPointName(shaderParams).data());
            }
        }

        for(const RaytracingPipelineStateShaderParams& shaderParams : mCallableShaderParams)
        {
            if(shaderParams.localRootSignatureIndex == localRootSignatureIndex)
            {
                ++structures.exportsAssociation.NumExports;
                localRootSignatureExports.push_back(getShaderEntryPointName(shaderParams).data());
            }
        }

        structures.exportsAssociation.pExports = localRootSignatureExports.data() + localRootSignatureExportsOffset;
        localRootSignatureExportsOffset = localRootSignatureExports.size();

        D3D12_STATE_SUBOBJECT& exportsAssociationSubObject = subObjects.emplace_back();
        exportsAssociationSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        exportsAssociationSubObject.pDesc = &structures.exportsAssociation;
    }
#pragma endregion

#pragma region pipeline_config
    // PIPELINE CONFIG
    auto* raytracingPipelineConfig = mMonotonicBuffer.allocate<D3D12_RAYTRACING_PIPELINE_CONFIG1>(1);
    raytracingPipelineConfig->MaxTraceRecursionDepth = mMaxRecursionDepth;
    raytracingPipelineConfig->Flags = D3D12_RAYTRACING_PIPELINE_FLAG_NONE;

    {
        D3D12_STATE_SUBOBJECT& configSubObject = subObjects.emplace_back();
        configSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
        configSubObject.pDesc = raytracingPipelineConfig;
    }
#pragma endregion
}

void RaytracingPipelineState::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    for(auto& localRootSignature : mLocalRootSignatures)
    {
        localRootSignature.markAsUsed(commandQueue);
    }
}

void RaytracingPipelineState::markAsUsed(ID3D12CommandList* commandList)
{
    for(auto& localRootSignature : mLocalRootSignatures)
    {
        localRootSignature.markAsUsed(commandList);
    }
}

void RaytracingPipelineState::setShaderIdentifier(RaytracingPipelineStage stage,
                                                  const RaytracingShaderIdentifier& shaderId)
{
    mFixedStageShaderIdentifiers[(size_t)stage] = shaderId;
}

void RaytracingPipelineState::setShaderIdentifier(size_t callableShaderIndex,
                                                  const RaytracingShaderIdentifier& shaderId)
{
    if(callableShaderIndex >= mCallableShaderParams.size()) { return; }
    mCallableShaderParams[callableShaderIndex].shaderIdentifier = shaderId;
}

RaytracingShaderIdentifier RaytracingPipelineState::getShaderIdentifier(RaytracingPipelineStage stage) const
{
    return mFixedStageShaderIdentifiers[(size_t)stage];
}

std::wstring_view
scrap::d3d12::RaytracingPipelineState::getFixedStageEntryPointName(const RaytracingPipelineStage stage) const
{
    switch(stage)
    {
    case RaytracingPipelineStage::RayGen:
        return getShaderEntryPointName(mFixedStageShaderParams[(size_t)RaytracingShaderStage::RayGen]);
    case RaytracingPipelineStage::HitGroup: return mHitGroupName;
    case RaytracingPipelineStage::Miss:
        return getShaderEntryPointName(mFixedStageShaderParams[(size_t)RaytracingShaderStage::Miss]);
    default: return {};
    }
}

std::wstring_view scrap::d3d12::RaytracingPipelineState::getCallableShaderEntryPointName(size_t index) const
{
    return getShaderEntryPointName(mCallableShaderParams[index]);
}

std::wstring_view
RaytracingPipelineState::getShaderEntryPointName(const RaytracingPipelineStateShaderParams& shaderParams) const
{
    return mShaders[shaderParams.shaderIndex]
        ->getFixedStageShaders()[shaderParams.shaderEntryPointIndex]
        .wideEntryPoint;
}
} // namespace scrap::d3d12