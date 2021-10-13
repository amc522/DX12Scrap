#pragma once

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <array>

#include <wrl/client.h>

enum D3D12_COMMAND_LIST_TYPE;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12GraphicsCommandList4;
struct ID3D12CommandQueue;

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

private:
    HRESULT appendNewAllocator(uint32_t frameIndex);

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mCommandList4;
    std::array<std::vector<TrackedGpuObject<ID3D12CommandAllocator>>, kFrameBufferCount> mCommandAllocators;
    D3D12_COMMAND_LIST_TYPE mCommandListType;
    uint32_t mFrameIndex = 0;
    std::wstring mDebugNameBase;
    std::wstring mDebugNameBuffer;
};
} // namespace scrap::d3d12