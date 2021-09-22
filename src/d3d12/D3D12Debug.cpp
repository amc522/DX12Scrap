#include "d3d12/D3D12Debug.h"

#include <cassert>

#include <d3d12.h>

#ifdef _DEBUG
#    include <filesystem>

#    include <DXProgrammableCapture.h>
#    include <ShlObj.h>
#    include <dxgi1_3.h>
#    include <dxgidebug.h>
#    include <pix3.h>
#    include <spdlog/spdlog.h>
#endif

using namespace Microsoft::WRL;

namespace scrap::d3d12
{
Debug* Debug::sInstance = nullptr;

Debug& Debug::instance()
{
    return *sInstance;
}

Debug::Debug()
{
    assert(sInstance == nullptr);
    sInstance = this;
}

#ifdef _DEBUG
thread_local std::string Debug::tStringBuffer;
thread_local std::wstring Debug::tWStringBuffer;

Debug::~Debug()
{
    assert(sInstance != nullptr);

    sInstance = nullptr;

    {
        ComPtr<ID3D12DebugDevice> debugDevice;
        if(SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(&debugDevice))))
        {
            mDevice.Reset();

            spdlog::info("ID3D12DebugDevice::ReportLiveDeviceObjects");

            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12sdklayers/nf-d3d12sdklayers-id3d12debugdevice-reportlivedeviceobjects
            if(FAILED(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL)))
            {
                spdlog::error("ReportLiveDeviceObjects failed");
            }

            // If the output looks like the following, there are no leaks. (1 ref on the device)
            //
            // clang-format off
        // D3D12 WARNING: Live ID3D12Device at 0x0000022ACF4D01A8, Refcount: 1 [ STATE_CREATION WARNING #274: LIVE_DEVICE]
            // clang-format on
        }
    }

    mDevice.Reset();

    { // Check for dxgi and d3d12 object leaks
        ComPtr<IDXGIDebug1> dxgiDebug;
        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-dxgigetdebuginterface1
        if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        {
            spdlog::info("IDXGIDebug1::ReportLiveObjects");

            // https://docs.microsoft.com/en-us/windows/win32/api/dxgidebug/nf-dxgidebug-idxgidebug-reportliveobjects
            HRESULT hr = dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL));
            if(FAILED(hr)) { spdlog::error("ReportLiveObjects failed"); }
        }
    }
}

void Debug::init(DebugOptions options)
{
    if((options & DebugOptions::EnableAttachToProcess) == DebugOptions::EnableAttachToProcess)
    {
        if(PIXLoadLatestWinPixGpuCapturerLibrary() != nullptr)
        {
            spdlog::info("Pix dll loaded. Pix can attach to process.");
            mIsPixAttached = true;
        }
        else
        {
            spdlog::error("Failed to load WinPixGpuCapturer.dll");
        }
    }
    else if(GetModuleHandle(L"WinPixGpuCapturer.dll") != 0)
    {
        mIsPixAttached = true;
        spdlog::info("Pix is attached");
    }

    // Always enable the debug layer before doing anything DX12 related
    // so all possible errors generated while creating DX12 objects
    // are caught by the debug layer.
    Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;

    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
    {
        debugInterface->EnableDebugLayer();
        spdlog::info("Enabled the d3d12 debug layer");
    }
    else
    {
        spdlog::error("Failed to enable the d3d12 debug layer.");
    }

    // https://docs.microsoft.com/en-us/windows/win32/api/dxgidebug/nn-dxgidebug-idxgiinfoqueue
    ComPtr<IDXGIInfoQueue> dxgiInfoQueue;

    // https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-dxgigetdebuginterface1
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
    {
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
        spdlog::info("Setting DXGI break on corruption and error");
    }
    else
    {
        spdlog::error("Failed to set DXGI break on corruption and error");
    }
}

void Debug::setDevice(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
    mDevice = std::move(device);

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12sdklayers/nn-d3d12sdklayers-id3d12infoqueue
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if(SUCCEEDED(mDevice.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        // pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // Suppress whole categories of messages
        // D3D12_MESSAGE_CATEGORY categories[] = {};

        // Suppress messages based on their severity level
        // D3D12_MESSAGE_SEVERITY severities[] = {};

        // Suppress individual messages by their ID
        // D3D12_MESSAGE_ID denyIds[] = {};

        // D3D12_INFO_QUEUE_FILTER newFilter = {};
        // newFilter.DenyList.NumCategories = _countof(categories);
        // newFilter.DenyList.pCategoryList = categories;
        // newFilter.DenyList.NumSeverities = _countof(severities);
        // newFilter.DenyList.pSeverityList = severities;
        // newFilter.DenyList.NumIDs = _countof(denyIds);
        // newFilter.DenyList.pIDList = denyIds;

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12sdklayers/nf-d3d12sdklayers-id3d12infoqueue-pushstoragefilter
        // pInfoQueue->PushStorageFilter(&newFilter);

        spdlog::info("Setting d3d12 break on corruption and error");
    }
    else
    {
        spdlog::error("Failed to set d3d12 break on corruption and error");
    }
}

void Debug::beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::string_view label)
{
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, label.data());
}

void Debug::beginGpuEvent(ID3D12GraphicsCommandList* commandList, std::wstring_view label)
{
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, label.data());
}

void Debug::beginGpuEvent(ID3D12CommandQueue* commandQueue, std::string_view label)
{
    PIXBeginEvent(commandQueue, PIX_COLOR_DEFAULT, label.data());
}

void Debug::beginGpuEvent(ID3D12CommandQueue* commandQueue, std::wstring_view label)
{
    PIXBeginEvent(commandQueue, PIX_COLOR_DEFAULT, label.data());
}

void Debug::endGpuEvent(ID3D12GraphicsCommandList* commandList)
{
    PIXEndEvent(commandList);
}

void Debug::endGpuEvent(ID3D12CommandQueue* commandQueue)
{
    PIXEndEvent(commandQueue);
}

void Debug::setGpuMarker(ID3D12GraphicsCommandList* commandList, std::string_view label)
{
    PIXSetMarker(commandList, PIX_COLOR_DEFAULT, label.data());
}

void Debug::setGpuMarker(ID3D12GraphicsCommandList* commandList, std::wstring_view label)
{
    PIXSetMarker(commandList, PIX_COLOR_DEFAULT, label.data());
}

void Debug::setGpuMarker(ID3D12CommandQueue* commandQueue, std::string_view label)
{
    PIXSetMarker(commandQueue, PIX_COLOR_DEFAULT, label.data());
}

void Debug::setGpuMarker(ID3D12CommandQueue* commandQueue, std::wstring_view label)
{
    PIXSetMarker(commandQueue, PIX_COLOR_DEFAULT, label.data());
}

void Debug::beginCapture()
{
    PIXCaptureParameters params;
    params.GpuCaptureParameters.FileName = L"pix_capture";
    params.TimingCaptureParameters = {};
    PIXBeginCapture(PIX_CAPTURE_GPU, &params);
}

void Debug::endCapture(bool discard)
{
    PIXEndCapture(discard);
}

ScopedGpuEvent::ScopedGpuEvent(ID3D12GraphicsCommandList* commandList, std::string_view label)
    : mContext(commandList)
    , mContextType(ContextType::GraphicsCommandList)
{
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, label.data());
}

ScopedGpuEvent::ScopedGpuEvent(ID3D12GraphicsCommandList* commandList, std::wstring_view label)
    : mContext(commandList)
    , mContextType(ContextType::GraphicsCommandList)
{
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, label.data());
}

ScopedGpuEvent::ScopedGpuEvent(ID3D12CommandQueue* commandQueue, std::string_view label)
    : mContext(commandQueue)
    , mContextType(ContextType::CommandQueue)
{
    PIXBeginEvent(commandQueue, PIX_COLOR_DEFAULT, label.data());
}

ScopedGpuEvent::ScopedGpuEvent(ID3D12CommandQueue* commandQueue, std::wstring_view label)
    : mContext(commandQueue)
    , mContextType(ContextType::CommandQueue)
{
    PIXBeginEvent(commandQueue, PIX_COLOR_DEFAULT, label.data());
}

ScopedGpuEvent::~ScopedGpuEvent()
{
    if(mContext != nullptr)
    {
        switch(mContextType)
        {
        case scrap::d3d12::ScopedGpuEvent::ContextType::GraphicsCommandList:
            PIXEndEvent(reinterpret_cast<ID3D12GraphicsCommandList*>(mContext));
            break;
        case scrap::d3d12::ScopedGpuEvent::ContextType::CommandQueue:
            PIXEndEvent(reinterpret_cast<ID3D12CommandQueue*>(mContext));
            break;
        case scrap::d3d12::ScopedGpuEvent::ContextType::Unknown: [[fallthrough]];
        default: PIXEndEvent(); break;
        }
    }
    else
    {
        PIXEndEvent();
    }
}
#else
Debug::~Debug()
{
    assert(sInstance != nullptr);
    sInstance = nullptr;
}

void Debug::init(DebugOptions) {}

void Debug::setDevice(Microsoft::WRL::ComPtr<ID3D12Device>) {}

void Debug::beginGpuEvent(ID3D12GraphicsCommandList*, std::string_view) {}

void Debug::beginGpuEvent(ID3D12GraphicsCommandList*, std::wstring_view) {}

void Debug::beginGpuEvent(ID3D12CommandQueue*, std::string_view) {}

void Debug::beginGpuEvent(ID3D12CommandQueue*, std::wstring_view) {}

void Debug::endGpuEvent(ID3D12GraphicsCommandList*) {}

void Debug::endGpuEvent(ID3D12CommandQueue*) {}

void Debug::setGpuMarker(ID3D12GraphicsCommandList*, std::string_view) {}

void Debug::setGpuMarker(ID3D12GraphicsCommandList*, std::wstring_view) {}

void Debug::setGpuMarker(ID3D12CommandQueue*, std::string_view) {}

void Debug::setGpuMarker(ID3D12CommandQueue*, std::wstring_view) {}

void Debug::beginCapture() {}

void Debug::endCapture(bool discard) {}

ScopedGpuEvent::ScopedGpuEvent(ID3D12GraphicsCommandList*, std::string_view) {}

ScopedGpuEvent::ScopedGpuEvent(ID3D12GraphicsCommandList*, std::wstring_view) {}

ScopedGpuEvent::ScopedGpuEvent(ID3D12CommandQueue*, std::string_view) {}

ScopedGpuEvent::ScopedGpuEvent(ID3D12CommandQueue*, std::wstring_view) {}

ScopedGpuEvent::~ScopedGpuEvent() {}
#endif
} // namespace scrap::d3d12