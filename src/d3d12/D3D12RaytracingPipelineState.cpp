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
    , mShader(std::move(params.shader))
{
    for(auto stage : enumerate<RaytracingShaderStage>())
    {
        const auto& inputParams = params.fixedStages[stage];

        if(!inputParams) { continue; }

        auto& internalParams = mFixedStageShaderParams[stage];
        internalParams.shaderEntryPointIndex = inputParams->shaderEntryPointIndex;

        if(inputParams->localRootSignature != nullptr)
        {
            auto itr =
                std::find(mLocalRootSignatures.begin(), mLocalRootSignatures.end(), inputParams->localRootSignature);
            if(itr == mLocalRootSignatures.end())
            {
                mLocalRootSignatures.emplace_back(std::move(inputParams->localRootSignature));
                itr = mLocalRootSignatures.end() - 1;
            }

            internalParams.localRootSignatureIndex = std::distance(mLocalRootSignatures.begin(), itr);
        }

        ++mValidFixedStageExportCount;
        mShaderStages |= RaytracingShaderStageToMask(stage);
    }

    mPipelineStages = RaytracingShaderStageMaskToPipelineMask(mShaderStages);

    mCallableShaderParams.reserve(mCallableShaderParams.size());
    for(size_t i = 0; i < params.callables.size(); ++i)
    {
        auto& inputParams = params.callables[i];

        size_t localRootSignatureIndex = std::numeric_limits<size_t>::max();
        if(inputParams.localRootSignature != nullptr)
        {
            auto itr =
                std::find(mLocalRootSignatures.begin(), mLocalRootSignatures.end(), inputParams.localRootSignature);
            if(itr == mLocalRootSignatures.end())
            {
                mLocalRootSignatures.emplace_back(std::move(inputParams.localRootSignature));
                itr = mLocalRootSignatures.end() - 1;
            }

            localRootSignatureIndex = (size_t)std::distance(mLocalRootSignatures.begin(), itr);
        }

        mCallableShaderParams.emplace_back(ShaderStageParams{.shaderEntryPointIndex = inputParams.shaderEntryPointIndex,
                                                             .localRootSignatureIndex = localRootSignatureIndex});
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
    if(mShader == nullptr) { return; }

    if(mShader->status() == RaytracingShaderState::Initialized) { mShader->create(); }

    while(mShader->status() == RaytracingShaderState::Compiling)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if(mShader->status() == RaytracingShaderState::Failed) { return; }

    struct LocalRootSignatureStructures
    {
        D3D12_LOCAL_ROOT_SIGNATURE signature;
        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION exportsAssociation;
    };

    const bool hasHitGroup = testEnumInMask(mPipelineStages, RaytracingPipelineStage::HitGroup);
    const size_t dxilLibraryDescCount = 1;
    const size_t hitGroupCount = (hasHitGroup) ? 1 : 0;
    const size_t exportDescsCount = mValidFixedStageExportCount + mCallableShaderParams.size();
    const size_t localRootSignatureCount = mLocalRootSignatures.size();
    const size_t localRootSignatureExportsCount =
        (localRootSignatureCount > 0) ? mValidFixedStageExportCount + mCallableShaderParams.size() : 0;

    const size_t subObjectCount = dxilLibraryDescCount + hitGroupCount +
                                  localRootSignatureCount * 2 // local root signatures and associations
                                  + 1;                        // pipeline config

    MemoryCalculator memoryCalc;
    memoryCalc.add<D3D12_STATE_SUBOBJECT>(subObjectCount);
    memoryCalc.add<D3D12_DXIL_LIBRARY_DESC>(dxilLibraryDescCount);
    memoryCalc.add<D3D12_EXPORT_DESC>(exportDescsCount);
    memoryCalc.add<D3D12_HIT_GROUP_DESC>(hitGroupCount);
    memoryCalc.add<LocalRootSignatureStructures>(localRootSignatureCount);
    memoryCalc.add<LPCWSTR>(localRootSignatureExportsCount);
    memoryCalc.add<D3D12_RAYTRACING_PIPELINE_CONFIG1>(1);

    mMonotonicBuffer = MonotonicBuffer(memoryCalc.getByteSize());

    {
        auto subObjects = makeMonotonicVector<D3D12_STATE_SUBOBJECT>(mMonotonicBuffer);
        subObjects.reserve(subObjectCount);

#pragma region dxil_libraries
        // DXIL LIBRARIES
        auto exportDescs = makeMonotonicVector<D3D12_EXPORT_DESC>(mMonotonicBuffer);
        exportDescs.reserve(exportDescsCount);

        ptrdiff_t exportOffset = 0;

        std::span fixedStageShaders = mShader->getFixedStageShaders();
        std::span callableShaders = mShader->getCallableShaders();

        for(auto stage : enumerate(mShaderStages))
        {
            const auto& shaderParams = mFixedStageShaderParams[stage];

            D3D12_EXPORT_DESC& desc = exportDescs.emplace_back();
            desc.Name = fixedStageShaders[shaderParams.shaderEntryPointIndex].wideEntryPoint.c_str();
        }

        for(const ShaderStageParams& shaderParams : mCallableShaderParams)
        {
            D3D12_EXPORT_DESC& desc = exportDescs.emplace_back();
            desc.Name = callableShaders[shaderParams.shaderEntryPointIndex].wideEntryPoint.c_str();
        }

        auto dxilLibDesc = mMonotonicBuffer.allocate<D3D12_DXIL_LIBRARY_DESC>(1);
        dxilLibDesc->DXILLibrary = mShader->getShaderByteCode();
        dxilLibDesc->NumExports = uint32_t(exportDescs.size() - exportOffset);
        dxilLibDesc->pExports = exportDescs.data() + exportOffset;

        exportOffset += dxilLibDesc->NumExports;

        D3D12_STATE_SUBOBJECT& subObject = subObjects.emplace_back();
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        subObject.pDesc = dxilLibDesc;

#pragma endregion

#pragma region hit_group
        // HIT GROUP
        if(hasHitGroup)
        {
            auto GetFixedStageEntryPointName = [&](RaytracingShaderStage stage) -> LPCWSTR {
                if(!testEnumInMask(mShaderStages, stage)) { return nullptr; }

                const auto& shaderParams = mFixedStageShaderParams[stage];
                std::span fixedStageShaders = mShader->getFixedStageShaders();

                return fixedStageShaders[shaderParams.shaderEntryPointIndex].wideEntryPoint.c_str();
            };

            D3D12_HIT_GROUP_DESC* hitGroupDesc = mMonotonicBuffer.allocate<D3D12_HIT_GROUP_DESC>(1);
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
        }
#pragma endregion

#pragma region local_root_signatures
        // LOCAL ROOT SIGNATURES

        auto localRootSignatureStructures = makeMonotonicVector<LocalRootSignatureStructures>(mMonotonicBuffer);
        localRootSignatureStructures.reserve(localRootSignatureCount);

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

            auto UsesCurrentLocalRootSignature = [localRootSignatureIndex](const ShaderStageParams& shaderParams) {
                return shaderParams.localRootSignatureIndex == localRootSignatureIndex;
            };

            for(auto stage : enumerate(mShaderStages))
            {
                const auto& shaderParams = mFixedStageShaderParams[stage];
                if(shaderParams.localRootSignatureIndex == localRootSignatureIndex)
                {
                    ++structures.exportsAssociation.NumExports;
                    localRootSignatureExports.push_back(getShaderEntryPointName(stage).data());
                }
            }

            for(size_t i = 0; i < mCallableShaderParams.size(); ++i)
            {
                const auto& shaderParams = mCallableShaderParams[i];
                if(shaderParams.localRootSignatureIndex == localRootSignatureIndex)
                {
                    ++structures.exportsAssociation.NumExports;
                    localRootSignatureExports.push_back(getCallableShaderEntryPointName(i).data());
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
        D3D12_RAYTRACING_PIPELINE_CONFIG1* raytracingPipelineConfig =
            mMonotonicBuffer.allocate<D3D12_RAYTRACING_PIPELINE_CONFIG1>(1);
        raytracingPipelineConfig->MaxTraceRecursionDepth = mMaxRecursionDepth;
        raytracingPipelineConfig->Flags = D3D12_RAYTRACING_PIPELINE_FLAG_NONE;

        {
            D3D12_STATE_SUBOBJECT& configSubObject = subObjects.emplace_back();
            configSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
            configSubObject.pDesc = raytracingPipelineConfig;
        }
#pragma endregion

        mSubObjects = subObjects;
    }
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
                                                  const RaytracingShaderIdentifier& shaderIdentifier)
{
    switch(stage)
    {
    case RaytracingPipelineStage::RayGen:
        mFixedStageShaderParams[RaytracingShaderStage::RayGen].shaderIdentifier = shaderIdentifier;
        break;
    case scrap::RaytracingPipelineStage::HitGroup: mHitGroupShaderIdentifier = shaderIdentifier; break;
    case RaytracingPipelineStage::Miss:
        mFixedStageShaderParams[RaytracingShaderStage::Miss].shaderIdentifier = shaderIdentifier;
        break;
    default: break;
    }
}

void RaytracingPipelineState::setCallableShaderIdentifier(size_t index,
                                                          const RaytracingShaderIdentifier& shaderIdentifier)
{
    mCallableShaderParams[index].shaderIdentifier = shaderIdentifier;
}

const RaytracingShaderIdentifier& RaytracingPipelineState::getShaderIdentifier(RaytracingPipelineStage stage) const
{
    switch(stage)
    {
    case RaytracingPipelineStage::RayGen:
        return mFixedStageShaderParams[RaytracingShaderStage::RayGen].shaderIdentifier;
    case scrap::RaytracingPipelineStage::HitGroup: return mHitGroupShaderIdentifier;
    case RaytracingPipelineStage::Miss: return mFixedStageShaderParams[RaytracingShaderStage::Miss].shaderIdentifier;
    default: return kInvalidShaderIdentifier;
    }
}

std::wstring_view
scrap::d3d12::RaytracingPipelineState::getShaderEntryPointName(const RaytracingShaderStage stage) const
{
    return getFixedStageShaderEntryPointName(mFixedStageShaderParams[stage]);
}

std::wstring_view RaytracingPipelineState::getShaderEntryPointName(RaytracingPipelineStage stage) const
{
    switch(stage)
    {
    case scrap::RaytracingPipelineStage::RayGen: return getShaderEntryPointName(RaytracingShaderStage::RayGen);
    case scrap::RaytracingPipelineStage::HitGroup: return mHitGroupName;
    case scrap::RaytracingPipelineStage::Miss: return getShaderEntryPointName(RaytracingShaderStage::Miss);
    default: return {};
    }
}

std::wstring_view scrap::d3d12::RaytracingPipelineState::getCallableShaderEntryPointName(size_t index) const
{
    return mShader->getCallableShaders()[mCallableShaderParams[index].shaderEntryPointIndex].wideEntryPoint;
}

std::wstring_view
RaytracingPipelineState::getFixedStageShaderEntryPointName(const ShaderStageParams& shaderParams) const
{
    return mShader->getFixedStageShaders()[shaderParams.shaderEntryPointIndex].wideEntryPoint;
}
} // namespace scrap::d3d12
