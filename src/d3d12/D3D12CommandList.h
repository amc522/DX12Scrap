#pragma once

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12Fwd.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <array>
#include <span>

#include <wrl/client.h>

namespace scrap::d3d12
{
class GraphicsCommandList
{
public:
    GraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, std::string_view debugName);
    GraphicsCommandList(const GraphicsCommandList&) = delete;
    GraphicsCommandList(GraphicsCommandList&&) = default;
    ~GraphicsCommandList() = default;

    GraphicsCommandList& operator=(const GraphicsCommandList&) = delete;
    GraphicsCommandList& operator=(GraphicsCommandList&&) = default;

    ID3D12GraphicsCommandList* get() const { return mCommandList.Get(); }
    ID3D12GraphicsCommandList4* get4() const { return mCommandList4.Get(); }

    HRESULT beginRecording();
    HRESULT execute(ID3D12CommandQueue* commandQueue);

    void endFrame();

    void setBindlessResource(uint32_t constantBufferIndex, uint32_t descriptorHeapIndex);

    [[nodiscard]] std::span<const uint32_t> getBindlessResourceIndices() const { return mBindlessResourceIndices; }

    void commitBindlessResources();

private:
    HRESULT appendNewAllocator(uint32_t frameIndex);

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mCommandList4;
    std::array<std::vector<TrackedGpuObject<ID3D12CommandAllocator>>, kFrameBufferCount> mCommandAllocators;
    D3D12_COMMAND_LIST_TYPE mCommandListType;
    uint32_t mFrameIndex = 0;
    std::wstring mDebugNameBase;
    std::wstring mDebugNameBuffer;
    std::array<uint32_t, d3d12::kMaxBindlessResources> mBindlessResourceIndices;
    uint32_t mBindlessChangeBegin = d3d12::kMaxBindlessResources;
    uint32_t mBindlessChangeEnd = 0;
};
} // namespace scrap::d3d12