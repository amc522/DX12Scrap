#include "d3d12/D3D12RaytracingDispatchPipelineState.h"

#include "EnumIterator.h"
#include "d3d12/D3D12CommandList.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12RaytracingPipelineState.h"

#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
void RaytracingDispatchPipelineState::init(RaytracingDispatchPipelineStateParams&& params)
{
    mMaxAttributeByteSize = params.maxAttributeByteSize;
    mMaxPayloadByteSize = params.maxPayloadByteSize;
    mGlobalRootSignature = TrackedGpuObject(std::move(params.globalRootSignature));
}

void RaytracingDispatchPipelineState::addPipelineState(std::shared_ptr<RaytracingPipelineState> pipelineState)
{
    {
        std::span stateSubObjects = pipelineState->getStateSubObjects();
        mTempStateSubObjects.resize(stateSubObjects.size() + 3);
        std::copy(stateSubObjects.begin(), stateSubObjects.end(), mTempStateSubObjects.begin());

        for(size_t i = 0; i < stateSubObjects.size(); ++i)
        {
            const auto& sourceSubObject = stateSubObjects[i];

            if(sourceSubObject.Type != D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION) { continue; }

            auto& destSubObject = mTempStateSubObjects[i];

            const auto* sourceDesc =
                reinterpret_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(sourceSubObject.pDesc);
            auto subObjectIndex = (sourceDesc->pSubobjectToAssociate - &stateSubObjects[0]);

            auto* destDesc =
                reinterpret_cast<D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(const_cast<void*>(destSubObject.pDesc));
            destDesc->pSubobjectToAssociate = &mTempStateSubObjects[subObjectIndex];
        }
    }

    // GLOBAL ROOT SIGNATURE
    D3D12_GLOBAL_ROOT_SIGNATURE globalRootSignatureDesc = {};
    globalRootSignatureDesc.pGlobalRootSignature = mGlobalRootSignature.get();

    D3D12_STATE_SUBOBJECT& globalRootSignatureSubObject = mTempStateSubObjects[mTempStateSubObjects.size() - 3];
    globalRootSignatureSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
    globalRootSignatureSubObject.pDesc = &globalRootSignatureDesc;

    // SHADER CONFIG
    D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
    shaderConfig.MaxPayloadSizeInBytes = mMaxPayloadByteSize;
    shaderConfig.MaxAttributeSizeInBytes = mMaxAttributeByteSize;

    D3D12_STATE_SUBOBJECT& shaderConfigSubObject = mTempStateSubObjects[mTempStateSubObjects.size() - 2];
    shaderConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
    shaderConfigSubObject.pDesc = &shaderConfig;

    // STATE OBJECT CONFIG
    D3D12_STATE_OBJECT_CONFIG stateObjectConfig = {};
    stateObjectConfig.Flags = D3D12_STATE_OBJECT_FLAG_ALLOW_STATE_OBJECT_ADDITIONS;

    D3D12_STATE_SUBOBJECT& stateObjectConfigSubObject = mTempStateSubObjects[mTempStateSubObjects.size() - 1];
    stateObjectConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG;
    stateObjectConfigSubObject.pDesc = &stateObjectConfig;

    // STATE OBJECT DESC
    D3D12_STATE_OBJECT_DESC stateObjectDesc = {};
    stateObjectDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    stateObjectDesc.NumSubobjects = (uint32_t)mTempStateSubObjects.size();
    stateObjectDesc.pSubobjects = mTempStateSubObjects.data();

    ComPtr<ID3D12StateObject> stateObject;
    HRESULT hr;

    if(mStateObject != nullptr)
    {
        hr = d3d12::DeviceContext::instance().getDevice7()->AddToStateObject(&stateObjectDesc, mStateObject.get(),
                                                                             IID_PPV_ARGS(&stateObject));
    }
    else
    {
        hr = d3d12::DeviceContext::instance().getDevice7()->CreateStateObject(&stateObjectDesc,
                                                                              IID_PPV_ARGS(&stateObject));
    }

    if(FAILED(hr)) { return; }

    mStateObject = TrackedGpuObject<ID3D12StateObject>(std::move(stateObject));

#pragma region collect_shader_identifiers
    {
        ComPtr<ID3D12StateObjectProperties> stateObjectProperties;

        if(FAILED(mStateObject->QueryInterface(IID_PPV_ARGS(&stateObjectProperties)))) { return; }

        auto makeShaderIdentifier = [&](std::wstring_view entryPointName) {
            const std::byte* bytes =
                reinterpret_cast<std::byte*>(stateObjectProperties->GetShaderIdentifier(entryPointName.data()));

            return RaytracingShaderIdentifier(std::span<const std::byte, RaytracingShaderIdentifier::kByteSize>(
                bytes, RaytracingShaderIdentifier::kByteSize));
        };

        for(auto stage : enumerate(pipelineState->getPipelineStages()))
        {
            const std::wstring_view entryPointName = pipelineState->getShaderEntryPointName(stage);
            pipelineState->setShaderIdentifier(stage, makeShaderIdentifier(entryPointName));
        }

        for(size_t callableIndex = 0; callableIndex < pipelineState->getCallableShaderCount(); ++callableIndex)
        {
            const std::wstring_view entryPointName = pipelineState->getCallableShaderEntryPointName(callableIndex);
            pipelineState->setCallableShaderIdentifier(callableIndex, makeShaderIdentifier(entryPointName));
        }
    }
#pragma endregion
}

void RaytracingDispatchPipelineState::markAsUsed(const GraphicsCommandList& commandList)
{
    mGlobalRootSignature.markAsUsed(commandList.get());
    mStateObject.markAsUsed(commandList.get());
}
} // namespace scrap::d3d12
