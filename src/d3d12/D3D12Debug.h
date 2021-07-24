#pragma once

#include "Utility.h"

#include <string_view>

#include <tl/expected.hpp>
#include <wrl/client.h>

struct ID3D12CommandList;
struct ID3D12CommandQueue;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct IDXGraphicsAnalysis;

namespace scrap::d3d12
{
enum class DebugOptions
{
    None = 0,
    EnableAttachToProcess = 1 << 0,
};
DEFINE_ENUM_BITWISE_OPERATORS(DebugOptions);

class Debug
{
public:
    static Debug& instance();

    Debug();
    ~Debug();

    void init(DebugOptions options);

    void setDevice(Microsoft::WRL::ComPtr<ID3D12Device> device);

    std::wstring getLatestWinPixGpuCapturerPath();
    bool loadWinPixDll();

    void beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::string_view label);
    void beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::wstring_view label);
    void beginGpuEvent(ID3D12CommandQueue* commandQueue, std::string_view label);
    void beginGpuEvent(ID3D12CommandQueue* commandQueue, std::wstring_view label);
    void endGpuEvent(ID3D12GraphicsCommandList* commandList);
    void endGpuEvent(ID3D12CommandQueue* commandQueue);

    void setGpuMarker(ID3D12GraphicsCommandList* commandList, std::string_view label);
    void setGpuMarker(ID3D12GraphicsCommandList* commandList, std::wstring_view label);
    void setGpuMarker(ID3D12CommandQueue* commandQueue, std::string_view label);
    void setGpuMarker(ID3D12CommandQueue* commandQueue, std::wstring_view label);

    void beginCapture();
    void endCapture();

    bool isPixAttached() const { return mIsPixAttached; }

private:
    static Debug* sInstance;
#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<IDXGraphicsAnalysis> mGraphicsAnalysis;
#endif
    bool mIsPixAttached = false;
};

class ScopedGpuEvent
{
public:
    explicit ScopedGpuEvent(ID3D12GraphicsCommandList* commandList, std::string_view label);
    explicit ScopedGpuEvent(ID3D12GraphicsCommandList* commandList, std::wstring_view label);
    explicit ScopedGpuEvent(ID3D12CommandQueue* commandQueue, std::string_view label);
    explicit ScopedGpuEvent(ID3D12CommandQueue* commandQueue, std::wstring_view label);
    ScopedGpuEvent(const ScopedGpuEvent&) = delete;
    ScopedGpuEvent(ScopedGpuEvent&&) = delete;
    ~ScopedGpuEvent();

    ScopedGpuEvent& operator=(const ScopedGpuEvent&) = delete;
    ScopedGpuEvent& operator=(ScopedGpuEvent&&) = delete;

private:
    enum class ContextType
    {
        GraphicsCommandList,
        CommandQueue,
        Unknown,
    };
#ifdef _DEBUG
    void* mContext = nullptr;
    ContextType mContextType = ContextType::Unknown;
#endif
};
} // namespace scrap::d3d12