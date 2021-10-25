#include "d3d12/D3D12RaytracingPipelineState.h"

#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12RaytracingShader.h"

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
RaytracingPipelineState::RaytracingPipelineState(RaytracingPipelineStateParams&& params)
    : mHitGroupName(Utf8ToWideString(params.hitGroupName))
    , mPrimitiveType(params.primitiveType)
    , mMaxAttributeByteSize(params.maxAttributeByteSize)
    , mMaxPayloadByteSize(params.maxPayloadByteSize)
    , mGlobalRootSignature(std::move(params.globalRootSignature))
    , mMaxRecursionDepth(params.maxRecursionDepth)
{
    mShaders.assign(params.shaders.begin(), params.shaders.end());

    mFixedStageShaderParams[(size_t)RaytracingShaderStage::RayGen] = params.fixedStages.raygen;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::Intersection] = params.fixedStages.intersection;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::AnyHit] = params.fixedStages.anyHit;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::ClosestHit] = params.fixedStages.closestHit;
    mFixedStageShaderParams[(size_t)RaytracingShaderStage::Miss] = params.fixedStages.miss;

    mValidFixedStageExportCount =
        (uint32_t)std::count_if(mFixedStageShaderParams.cbegin(), mFixedStageShaderParams.cend(),
                                [](const auto& shaderExport) { return shaderExport.hasValidShaderEntryPoint(); });

    mCallableShaderParams.assign(params.callables.cbegin(), params.callables.cend());

    mLocalRootSignatures.reserve(params.localRootSignatures.size());
    for(ComPtr<ID3D12RootSignature>& localRootSignature : params.localRootSignatures)
    {
        mLocalRootSignatures.emplace_back(std::move(localRootSignature));
    }
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

    const size_t subObjectCount = mShaders.size()                   // dxil libraries
                                  + 1                               // hit group
                                  + 1                               // shader config
                                  + 1                               // global root signature
                                  + mLocalRootSignatures.size() * 2 // local root signatures and associations
                                  + 1;                              // pipeline config

    std::vector<D3D12_STATE_SUBOBJECT> subObjects;
    subObjects.reserve(subObjectCount);

#pragma region dxil_libraries
    // DXIL LIBRARIES
    std::vector<D3D12_DXIL_LIBRARY_DESC> dxilLibDescs;
    dxilLibDescs.reserve(mShaders.size());

    std::vector<D3D12_EXPORT_DESC> exportDescs;
    exportDescs.reserve(mValidFixedStageExportCount + mCallableShaderParams.size());

    ptrdiff_t exportOffset = 0;

    for(size_t shaderIndex = 0; shaderIndex < mShaders.size(); ++shaderIndex)
    {
        const std::shared_ptr<RaytracingShader>& shader = mShaders[shaderIndex];
        nonstd::span fixedStageShaders = shader->getFixedStageShaders();
        nonstd::span callableShaders = shader->getCallableShaders();

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

        nonstd::span fixedStageShaders = mShaders[shaderParams.shaderIndex]->getFixedStageShaders();

        return fixedStageShaders[shaderParams.shaderEntryPointIndex].wideEntryPoint.c_str();
    };

    D3D12_HIT_GROUP_DESC hitGroupDesc = {};
    hitGroupDesc.IntersectionShaderImport = GetFixedStageEntryPointName(RaytracingShaderStage::Intersection);
    hitGroupDesc.AnyHitShaderImport = GetFixedStageEntryPointName(RaytracingShaderStage::AnyHit);
    hitGroupDesc.ClosestHitShaderImport = GetFixedStageEntryPointName(RaytracingShaderStage::ClosestHit);
    hitGroupDesc.HitGroupExport = mHitGroupName.c_str();
    hitGroupDesc.Type = (mPrimitiveType == RaytracingPipelineStatePrimitiveType::Triangles)
                            ? D3D12_HIT_GROUP_TYPE_TRIANGLES
                            : D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects.emplace_back();
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        subObject.pDesc = &hitGroupDesc;
    }
#pragma endregion

#pragma region shader_config
    // SHADER CONFIG
    D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
    shaderConfig.MaxPayloadSizeInBytes = mMaxPayloadByteSize;
    shaderConfig.MaxAttributeSizeInBytes = mMaxAttributeByteSize;

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects.emplace_back();
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        subObject.pDesc = &shaderConfig;
    }
#pragma endregion

#pragma region local_root_signatures
    // LOCAL ROOT SIGNATURES
    struct LocalRootSignatureStructures
    {
        D3D12_LOCAL_ROOT_SIGNATURE signature;
        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION exportsAssociation;
    };

    eastl::fixed_vector<LocalRootSignatureStructures, 1> localRootSignatureStructures;
    localRootSignatureStructures.reserve(mLocalRootSignatures.size());

    eastl::fixed_vector<LPCWSTR, (size_t)RaytracingShaderStage::Count> localRootSignatureExports;
    localRootSignatureExports.reserve(mValidFixedStageExportCount + mCallableShaderParams.size());
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

#pragma region global_root_signature
    // GLOBAL ROOT SIGNATURE
    D3D12_GLOBAL_ROOT_SIGNATURE globalRootSignatureDesc = {};
    globalRootSignatureDesc.pGlobalRootSignature = mGlobalRootSignature.get();

    {
        D3D12_STATE_SUBOBJECT& globalRootSignatureSubObject = subObjects.emplace_back();
        globalRootSignatureSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
        globalRootSignatureSubObject.pDesc = &globalRootSignatureDesc;
    }
#pragma endregion

#pragma region pipeline_config
    // PIPELINE CONFIG
    D3D12_RAYTRACING_PIPELINE_CONFIG1 raytracingPipelineConfig = {};
    raytracingPipelineConfig.MaxTraceRecursionDepth = mMaxRecursionDepth;
    raytracingPipelineConfig.Flags = D3D12_RAYTRACING_PIPELINE_FLAG_NONE;

    {
        D3D12_STATE_SUBOBJECT& configSubObject = subObjects.emplace_back();
        configSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
        configSubObject.pDesc = &raytracingPipelineConfig;
    }
#pragma endregion

#pragma region state_object_desc
    // CREATE STATE OBJECT
    D3D12_STATE_OBJECT_DESC raytracingPipelineDesc = {};
    raytracingPipelineDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    raytracingPipelineDesc.NumSubobjects = (uint32_t)subObjects.size();
    raytracingPipelineDesc.pSubobjects = subObjects.data();
#pragma endregion

    ComPtr<ID3D12StateObject> stateObject;
    HRESULT hr = d3d12::DeviceContext::instance().getDevice5()->CreateStateObject(&raytracingPipelineDesc,
                                                                                  IID_PPV_ARGS(&stateObject));

    if(FAILED(hr)) { return; }

    mStateObject = TrackedGpuObject(std::move(stateObject));
}

void RaytracingPipelineState::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    mStateObject.markAsUsed(commandQueue);
    mGlobalRootSignature.markAsUsed(commandQueue);

    for(auto& localRootSignature : mLocalRootSignatures)
    {
        localRootSignature.markAsUsed(commandQueue);
    }
}

void RaytracingPipelineState::markAsUsed(ID3D12CommandList* commandList)
{
    mStateObject.markAsUsed(commandList);
    mGlobalRootSignature.markAsUsed(commandList);

    for(auto& localRootSignature : mLocalRootSignatures)
    {
        localRootSignature.markAsUsed(commandList);
    }
}

std::wstring_view
RaytracingPipelineState::getShaderEntryPointName(const RaytracingPipelineStateShaderParams& shaderParams) const
{
    return mShaders[shaderParams.shaderIndex]
        ->getFixedStageShaders()[shaderParams.shaderEntryPointIndex]
        .wideEntryPoint;
}
} // namespace scrap::d3d12