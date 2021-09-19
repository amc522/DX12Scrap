#pragma once

#include "Utility.h"

#include <string>
#include <string_view>

#include <fmt/format.h>
#include <tl/expected.hpp>
#include <wrl/client.h>

struct ID3D12CommandList;
struct ID3D12CommandQueue;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;

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

    template<class... Args>
    void beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::string_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tStringBuffer), format, std::forward(args)...);
        beginGpuEvent(commandList, tStringBuffer);
        tStringBuffer.clear();
#endif
    }

    template<class... Args>
    void beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::wstring_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tWStringBuffer), format, std::forward(args)...);
        beginGpuEvent(commandList, tWStringBuffer);
        tWStringBuffer.clear();
#endif
    }

    template<class... Args>
    void beginGpuEvent(ID3D12CommandQueue* commandQueue, std::string_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tStringBuffer), format, std::forward<Args>(args)...);
        beginGpuEvent(commandQueue, tStringBuffer);
        tStringBuffer.clear();
#endif
    }

    template<class... Args>
    void beginGpuEvent(ID3D12CommandQueue* commandQueue, std::wstring_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tWStringBuffer), format, std::forward(args)...);
        beginGpuEvent(commandQueue, tWStringBuffer);
        tWStringBuffer.clear();
#endif
    }

    void beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::string_view label);
    void beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::wstring_view label);
    void beginGpuEvent(ID3D12CommandQueue* commandQueue, std::string_view label);
    void beginGpuEvent(ID3D12CommandQueue* commandQueue, std::wstring_view label);
    void endGpuEvent(ID3D12GraphicsCommandList* commandList);
    void endGpuEvent(ID3D12CommandQueue* commandQueue);

    template<class... Args>
    void setGpuMarker(ID3D12GraphicsCommandList* commandList, std::string_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tStringBuffer), format, std::forward(args)...);
        setGpuMarker(commandList, tStringBuffer);
        tStringBuffer.clear();
#endif
    }

    template<class... Args>
    void setGpuMarker(ID3D12GraphicsCommandList* commandList, std::wstring_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tWStringBuffer), format, std::forward(args)...);
        setGpuMarker(commandList, tWStringBuffer);
        tWStringBuffer.clear();
#endif
    }

    template<class... Args>
    void setGpuMarker(ID3D12CommandQueue* commandQueue, std::string_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tStringBuffer), format, std::forward(args)...);
        setGpuMarker(commandQueue, tStringBuffer);
        tStringBuffer.clear();
#endif
    }

    template<class... Args>
    void setGpuMarker(ID3D12CommandQueue* commandQueue, std::wstring_view format, Args&&... args)
    {
#ifdef _DEBUG
        fmt::format_to(std::back_inserter(tWStringBuffer), format, std::forward(args)...);
        setGpuMarker(commandQueue, tWStringBuffer);
        tWStringBuffer.clear();
#endif
    }

    void setGpuMarker(ID3D12GraphicsCommandList* commandList, std::string_view label);
    void setGpuMarker(ID3D12GraphicsCommandList* commandList, std::wstring_view label);
    void setGpuMarker(ID3D12CommandQueue* commandQueue, std::string_view label);
    void setGpuMarker(ID3D12CommandQueue* commandQueue, std::wstring_view label);

    void beginCapture();
    void endCapture(bool discard = false);

    bool isPixAttached() const { return mIsPixAttached; }

private:
    static Debug* sInstance;
#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    thread_local static std::string tStringBuffer;
    thread_local static std::wstring tWStringBuffer;
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