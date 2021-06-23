#include "D3D12Context.h"

#include "D3D12Strings.h"
#include "Window.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#    include <dxgidebug.h>
#endif
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap
{
std::wstring_view GetGpuVendorName(UINT vendorId)
{
    switch(vendorId)
    {
    case 0x1002: return L"AMD";
    case 0x10DE: return L"Nvidia";
    case 0x8086: return L"Intel";
    default: return L"Unknown";
    }
}

void LogAdapterDesc(const DXGI_ADAPTER_DESC1& desc, UINT adapterIndex)
{
    spdlog::info(L"Adapter {}\n\t{}\n\tVendor: {} ({:#x})\n\tDevice Id: {:#x}\n\tDedicated Video Memory: "
                 L"{:L} bytes\n\tDedicated System Memory: {:L} bytes\n\tShared System Memory: {:L} bytes",
                 adapterIndex, desc.Description, GetGpuVendorName(desc.VendorId), desc.VendorId, desc.DeviceId,
                 desc.DedicatedVideoMemory, desc.DedicatedSystemMemory, desc.SharedSystemMemory);
}

D3D12Context::D3D12Context(const Window& window, GpuPreference gpuPreference)
{
    spdlog::info("Initializing D3D12");

#ifdef _DEBUG
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
#endif

    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#ifdef _DEBUG
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    if(FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4))))
    {
        spdlog::critical("CreateDXGIFactory2 failed.");
        return;
    }

#ifdef _DEBUG
    // https://docs.microsoft.com/en-us/windows/win32/api/dxgidebug/nn-dxgidebug-idxgiinfoqueue
    ComPtr<IDXGIInfoQueue> dxgiInfoQueue;

    // https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-dxgigetdebuginterface1
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
    {
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
    }
#endif

    // Feature level documentation
    // https://docs.microsoft.com/en-us/windows/win32/direct3d12/hardware-feature-levels
    constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    getHardwareAdapter(gpuPreference, featureLevel, dxgiFactory4.Get());

    createDevice(featureLevel);

#ifdef _DEBUG
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
    }
#endif

    { // create command queue
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
        commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandqueue
        if(FAILED(mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue))))
        {
            spdlog::critical("Failed to create d3d12 command queue");
            return;
        }

        spdlog::info("Created d3d12 command queue");
    }

    { // create swap chain
        const glm::i32vec2 frameSize = window.getDrawableSize();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Width = frameSize.x;
        swapChainDesc.Height = frameSize.y;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;

        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/nf-dxgi1_2-idxgifactory2-createswapchainforhwnd
        if(FAILED(dxgiFactory4->CreateSwapChainForHwnd(
               mCommandQueue.Get(), // Swap chain needs the queue so that it can force a flush on it.
               window.getHwnd(), &swapChainDesc, nullptr, nullptr, &swapChain)))
        {
            spdlog::critical("Failed to create d3d12 swap chain");
            return;
        }

        // Currently do not support fullscreen transitions.
        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory-makewindowassociation
        if(FAILED(dxgiFactory4->MakeWindowAssociation(window.getHwnd(), DXGI_MWA_NO_ALT_ENTER)))
        {
            spdlog::error("Failed to disable alt-enter fullscreen binding");
        }

        if(FAILED(swapChain.As(&mSwapChain))) { spdlog::critical("Failed to convert swap chain"); }

        mFrameSize = frameSize;
        mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

        spdlog::info("Created d3d12 swap chain");
    }

    { // Create descriptor heaps.
        // https://docs.microsoft.com/en-us/windows/win32/direct3d12/creating-descriptor-heaps

        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = sFrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createdescriptorheap
        if(FAILED(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap))))
        {
            spdlog::critical("Failed to create rtv descriptor heap");
            return;
        }

        spdlog::info("Created rtv descriptor heap");

        mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // Describe and create a shader resource view (SRV) heap for the texture.
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if(FAILED(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvHeap))))
        {
            spdlog::critical("Failed to create srv descriptor heap");
            return;
        }

        spdlog::info("Created srv descriptor heap");

        mSrvDescriptorHeapSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    { // Create frame resources.
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for(UINT frameIndex = 0; frameIndex < sFrameCount; frameIndex++)
        {
            if(FAILED(mSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&mRenderTargets[frameIndex]))))
            {
                spdlog::critical("Failed to get render target buffer for frame {}", frameIndex);
                return;
            }

            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createrendertargetview
            mDevice->CreateRenderTargetView(mRenderTargets[frameIndex].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, mRtvDescriptorSize);
        }

        spdlog::info("Created render target views");
    }

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandallocator
    if(FAILED(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator))))
    {
        spdlog::critical("Failed to create d3d12 command allocator");
        return;
    }

    spdlog::info("Created d3d12 command allocator");

    mInitialized = true;
}

D3D12Context::~D3D12Context()
{
    spdlog::info("Destroying D3D12");

#ifdef _DEBUG
    if(mDevice != nullptr)
    {
        mCommandAllocator.Reset();
        mCommandQueue.Reset();
        mRenderTargets.fill(nullptr);
        mRtvHeap.Reset();
        mSrvHeap.Reset();
        mSwapChain.Reset();

        { // Check for d3d12 object leaks
            ComPtr<ID3D12DebugDevice> debugDevice;
            if(SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(&debugDevice))))
            {
                mDevice.Reset();
                mDevice1.Reset();
                mDevice2.Reset();
                mDevice3.Reset();
                mDevice4.Reset();
                mDevice5.Reset();
                mDevice6.Reset();

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
        mDevice1.Reset();
        mDevice2.Reset();
        mDevice3.Reset();
        mDevice4.Reset();
        mDevice5.Reset();
        mDevice6.Reset();

        mAdapter.Reset();

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
#endif
}

constexpr DXGI_GPU_PREFERENCE GetDxgiGpuPreference(GpuPreference gpuPreference)
{
    switch(gpuPreference)
    {
    case scrap::GpuPreference::HighPerformance: return DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    case scrap::GpuPreference::MinimumPower: return DXGI_GPU_PREFERENCE_MINIMUM_POWER;
    case scrap::GpuPreference::MaximumMemory: [[fallthrough]];
    case scrap::GpuPreference::MinimumMemory: [[fallthrough]];
    case scrap::GpuPreference::None: [[fallthrough]];
    default: return DXGI_GPU_PREFERENCE_UNSPECIFIED;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Context::getBackBufferRtv() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex,
                                         mRtvDescriptorSize);
}

void D3D12Context::present()
{
    if(FAILED(mSwapChain->Present(1, 0))) { spdlog::error("Present call failed"); }
}

void D3D12Context::swap() { mFrameIndex = mSwapChain->GetCurrentBackBufferIndex(); }

void D3D12Context::getHardwareAdapter(GpuPreference gpuPreference,
                                      D3D_FEATURE_LEVEL featureLevel,
                                      IDXGIFactory4* dxgiFactory4)
{
    const DXGI_GPU_PREFERENCE dxgiGpuPreference = GetDxgiGpuPreference(gpuPreference);

    spdlog::info("Enumerating adapters with preference '{}'", gpuPreference);

    ComPtr<IDXGIAdapter4> dxgiAdapter4;
    UINT selectedAdapterIndex = 0;

    ComPtr<IDXGIFactory6> dxgiFactory6;
    if(dxgiGpuPreference != DXGI_GPU_PREFERENCE_UNSPECIFIED &&
       SUCCEEDED(dxgiFactory4->QueryInterface(IID_PPV_ARGS(&dxgiFactory6))))
    {
        ComPtr<IDXGIAdapter1> dxgiAdapter1;

        for(UINT adapterIndex = 0;
            dxgiFactory6->EnumAdapterByGpuPreference(adapterIndex, dxgiGpuPreference, IID_PPV_ARGS(&dxgiAdapter1)) !=
            DXGI_ERROR_NOT_FOUND;
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            if((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) { continue; }

            LogAdapterDesc(dxgiAdapterDesc1, adapterIndex);

            if(FAILED(D3D12CreateDevice(dxgiAdapter1.Get(), featureLevel, __uuidof(ID3D12Device), nullptr)))
            {
                spdlog::info("Adapter {} does not support feature level {}", adapterIndex, featureLevel);
                continue;
            }

            if(FAILED(dxgiAdapter1.As(&dxgiAdapter4))) { continue; }

            selectedAdapterIndex = adapterIndex;

            // The adapters are ordered based on dxgiGpuPrefence. The first one should be good.
            break;
        }
    }
    else
    {
        ComPtr<IDXGIAdapter1> dxgiAdapter1;

        SIZE_T selectedDedicatedVideoMemory =
            (gpuPreference == GpuPreference::MaximumMemory) ? 0u : std::numeric_limits<SIZE_T>::max();

        for(UINT adapterIndex = 0; dxgiFactory4->EnumAdapters1(adapterIndex, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND;
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // Check if this is a software adapter. If it is, just skip it.
            if((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) { continue; }

            LogAdapterDesc(dxgiAdapterDesc1, adapterIndex);

            if(FAILED(D3D12CreateDevice(dxgiAdapter1.Get(), featureLevel, __uuidof(ID3D12Device), nullptr)))
            {
                spdlog::info("Adapter {} does not support feature level {}", adapterIndex, featureLevel);
                continue;
            }

            if((gpuPreference == GpuPreference::MaximumMemory &&
                dxgiAdapterDesc1.DedicatedVideoMemory < selectedDedicatedVideoMemory) ||
               (gpuPreference == GpuPreference::MinimumMemory &&
                dxgiAdapterDesc1.DedicatedVideoMemory > selectedDedicatedVideoMemory))
            {
                continue;
            }

            if(FAILED(dxgiAdapter1.As(&dxgiAdapter4))) { continue; }

            selectedDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
            selectedAdapterIndex = adapterIndex;
        }
    }

    if(dxgiAdapter4 == nullptr)
    {
        spdlog::critical("Failed to find a support adapter.");
        return;
    }

    mAdapter = std::move(dxgiAdapter4);
    spdlog::info("Using adapter {}", selectedAdapterIndex);
}

HRESULT D3D12Context::createDevice(D3D_FEATURE_LEVEL featureLevel)
{
    if(FAILED(D3D12CreateDevice(mAdapter.Get(), featureLevel, IID_PPV_ARGS(&mDevice))))
    {
        spdlog::critical("Failed to create d3d12 device");
        return E_FAIL;
    }

    spdlog::info("Created d3d12 device");

    ComPtr<ID3D12Device1> device1;
    if(FAILED(mDevice->QueryInterface(IID_PPV_ARGS(&device1)))) { return S_OK; }
    mDevice1 = std::move(device1);
    spdlog::info("Create d3d12 device1");

    ComPtr<ID3D12Device2> device2;
    if(FAILED(mDevice->QueryInterface(IID_PPV_ARGS(&device2)))) { return S_OK; }
    mDevice2 = std::move(device2);
    spdlog::info("Create d3d12 device2");

    ComPtr<ID3D12Device3> device3;
    if(FAILED(mDevice->QueryInterface(IID_PPV_ARGS(&device3)))) { return S_OK; }
    mDevice3 = std::move(device3);
    spdlog::info("Create d3d12 device3");

    ComPtr<ID3D12Device4> device4;
    if(FAILED(mDevice->QueryInterface(IID_PPV_ARGS(&device4)))) { return S_OK; }
    mDevice4 = std::move(device4);
    spdlog::info("Create d3d12 device4");

    ComPtr<ID3D12Device5> device5;
    if(FAILED(mDevice->QueryInterface(IID_PPV_ARGS(&device5)))) { return S_OK; }
    mDevice5 = std::move(device5);
    spdlog::info("Create d3d12 device5");

    ComPtr<ID3D12Device6> device6;
    if(FAILED(mDevice->QueryInterface(IID_PPV_ARGS(&device6)))) { return S_OK; }
    mDevice6 = std::move(device6);
    spdlog::info("Create d3d12 device6");

    return S_OK;
}

} // namespace scrap