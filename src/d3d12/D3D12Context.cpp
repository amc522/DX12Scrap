#include "D3D12Context.h"

#include "D3D12Debug.h"
#include "D3D12Strings.h"
#include "Window.h"
#include "d3d12/D3D12GraphicsPipelineState.h"

#include <chrono>

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <gpufmt/dxgi.h>
#include <spdlog/spdlog.h>

// These exports are needed for the DX12 Agility SDK. The agility sdk is a portable dx12 binary to be shipped with the
// executable. It allows for later dx12 versions than the host machine might have installed.
//
// The DX12 Agility SDK is currently downloaded and installed for development through NuGet in the Visual Studio
// project. Everything should happen automagically.
//
// https://devblogs.microsoft.com/directx/gettingstarted-dx12agility/#howto
extern "C"
{
    __declspec(dllexport) extern const UINT D3D12SDKVersion = 600;
    __declspec(dllexport) extern const char8_t* D3D12SDKPath = u8".\\D3D12\\";
}

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

namespace d3d12
{
DeviceContext* DeviceContext::sInstance = nullptr;

DeviceContext::DeviceContext(const Window& window, GpuPreference gpuPreference)
{
    assert(sInstance == nullptr);
    sInstance = this;

    spdlog::info("Initializing D3D12");

    mDebug.init(DebugOptions::EnableAttachToProcess);

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

    // Feature level documentation
    // https://docs.microsoft.com/en-us/windows/win32/direct3d12/hardware-feature-levels
    constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    getHardwareAdapter(gpuPreference, featureLevel, dxgiFactory4.Get());

    if(FAILED(createDevice(featureLevel))) { return; }

    mDebug.setDevice(mDevice);

    if(!checkFeatureSupport()) { return; }

    mGraphicsContext = std::make_unique<GraphicsContext>();
    mGraphicsContext->init();

    { // create swap chain
        const glm::i32vec2 frameSize = window.getDrawableSize();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = kFrameBufferCount;
        swapChainDesc.Width = frameSize.x;
        swapChainDesc.Height = frameSize.y;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;

        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/nf-dxgi1_2-idxgifactory2-createswapchainforhwnd
        if(FAILED(dxgiFactory4->CreateSwapChainForHwnd(
               mGraphicsContext->getCommandQueue(), // Swap chain needs the queue so that it can force a flush on it.
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

        mFrameBufferSize = frameSize;
        mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

        spdlog::info("Created d3d12 swap chain");
    }

    { // Create descriptor heaps.
        // https://docs.microsoft.com/en-us/windows/win32/direct3d12/creating-descriptor-heaps
        mSwapChainRtvHeap = std::make_unique<MonotonicDescriptorHeap_RTV>(*this, (uint32_t)d3d12::kFrameBufferCount);
        if(!mSwapChainRtvHeap->isValid())
        {
            spdlog::critical("Failed to create RTV descriptor heap");
            return;
        }

        spdlog::info("Created permanent RTV descriptor heap with {} descriptors. {} bytes",
                     mSwapChainRtvHeap->getDesctiptorCount(), mSwapChainRtvHeap->getByteSize());

        // Create a shader resource view (SRV) heap
        mCbvSrvUavHeap = std::make_unique<FixedDescriptorHeap_CBV_SRV_UAV>(*this, 1024);
        if(!mCbvSrvUavHeap->isValid())
        {
            spdlog::critical("Failed to create CBV/SRV/UAV descriptor heap.");
            return;
        }

        spdlog::info("Created fixed CBV/SRV/UAV descriptor heap with {} descriptors. {} bytes",
                     mCbvSrvUavHeap->getDesctiptorCount(), mCbvSrvUavHeap->getByteSize());

        // Create a render target view (RTV) heap
        mRtvHeap = std::make_unique<FixedDescriptorHeap_RTV>(*this, 16);
        if(!mRtvHeap->isValid())
        {
            spdlog::critical("Failed to create RTV descriptor heap.");
            return;
        }

        spdlog::info("Created fixed RTV descriptor heap with {} descriptors. {} bytes", mRtvHeap->getDesctiptorCount(),
                     mRtvHeap->getByteSize());

        // Create a depth stencil view (DSV) heap
        mDsvHeap = std::make_unique<FixedDescriptorHeap_DSV>(*this, 16);
        if(!mDsvHeap->isValid())
        {
            spdlog::critical("Failed to create DSV descriptor heap.");
            return;
        }

        spdlog::info("Created fixed DSV descriptor heap with {} descriptors. {} bytes", mDsvHeap->getDesctiptorCount(),
                     mDsvHeap->getByteSize());
    }

    { // Create frame resources.
        auto rtvDescriptors = mSwapChainRtvHeap->allocate(kFrameBufferCount);

        if(!rtvDescriptors)
        {
            spdlog::critical("Failed to create render target descriptors");
            return;
        }

        mSwapChainRtvs = std::move(rtvDescriptors.value());

        for(uint32_t frameIndex = 0; frameIndex < d3d12::kFrameBufferCount; ++frameIndex)
        {
            if(FAILED(mSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&mRenderTargets[frameIndex]))))
            {
                spdlog::critical("Failed to get render target buffer for frame {}", frameIndex);
                return;
            }

            mSwapChainRtvHeap->createRenderTargetView(*this, mSwapChainRtvs, frameIndex,
                                                      mRenderTargets[frameIndex].Get(), nullptr);
        }

        spdlog::info("Created render target views");
    }

    mCopyContext = std::make_unique<CopyContext>();
    mCopyContext->init();
    mCopyContext->beginFrame();

    mInitialized = true;
}

DeviceContext::~DeviceContext()
{
    spdlog::info("Destroying D3D12");

    for(auto& future : mFutures)
    {
        if(!future.valid()) { continue; }
        future.wait();
    }
    mFutures.clear();

    if(mCopyContext != nullptr) { mCopyContext->releaseResources(); }

    assert(sInstance != nullptr);
    sInstance = nullptr;
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

D3D12_CPU_DESCRIPTOR_HANDLE DeviceContext::getBackBufferRtv() const
{
    return mSwapChainRtvs.getCpuHandle(mFrameIndex);
}

void DeviceContext::beginFrame()
{
    mGraphicsContext->beginFrame();

    mCbvSrvUavHeap->uploadPendingDescriptors(*this);

    mFutures.erase(std::remove_if(mFutures.begin(), mFutures.end(),
                                  [](const std::future<void>& future) {
                                      return future.valid() &&
                                             future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                                  }),
                   mFutures.end());

    mCopyContext->endFrame();
    mCopyContext->beginFrame();
}

void DeviceContext::endFrame()
{
    HRESULT hr = mSwapChain->Present(1, 0);
    if(FAILED(hr))
    {
        spdlog::error("Present call failed {}", HRESULT_t(hr));

        if(hr == DXGI_ERROR_DEVICE_REMOVED) { mDebug.handleDeviceRemoved(); }

        return;
    }

    // Update the frame index.
    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    mGraphicsContext->endFrame();
}

std::shared_ptr<GraphicsPipelineState> DeviceContext::createGraphicsPipelineState(GraphicsPipelineStateParams&& params)
{
    auto pipelineState = std::make_shared<GraphicsPipelineState>(std::move(params));
    // This is a really cheesy way to do multithreaded creation of pipeline states. In the future, I would like to make
    // some kind of task manager for handling async resource creation
    mFutures.emplace_back(std::async(std::launch::async, [pipelineState]() { pipelineState->create(); }));

    return pipelineState;
}

void DeviceContext::getHardwareAdapter(GpuPreference gpuPreference,
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

HRESULT DeviceContext::createDevice(D3D_FEATURE_LEVEL featureLevel)
{
    if(FAILED(D3D12CreateDevice(mAdapter.Get(), featureLevel, IID_PPV_ARGS(&mDevice))))
    {
        spdlog::critical("Failed to create d3d12 device with {}", featureLevel);
        return E_FAIL;
    }

    spdlog::info("Created d3d12 device with {}", featureLevel);

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

    ComPtr<ID3D12Device7> device7;
    if(FAILED(mDevice->QueryInterface(IID_PPV_ARGS(&device7)))) { return S_OK; }
    mDevice7 = std::move(device7);
    spdlog::info("Create d3d12 device7");

    return S_OK;
}

bool DeviceContext::checkFeatureSupport()
{
    { // check for hlsl dynamic resources support
        // https://microsoft.github.io/DirectX-Specs/d3d/HLSL_SM_6_6_DynamicResources.html
        D3D12_FEATURE_DATA_D3D12_OPTIONS optionsData = {};
        if(SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &optionsData, sizeof(optionsData))))
        {
            spdlog::info("{}", optionsData.ResourceBindingTier);
        }

        D3D12_FEATURE_DATA_SHADER_MODEL shaderModelData{D3D_SHADER_MODEL_6_6};
        if(SUCCEEDED(
               mDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModelData, sizeof(shaderModelData))))
        {
            spdlog::info("{}", shaderModelData.HighestShaderModel);
        }

        if(optionsData.ResourceBindingTier < D3D12_RESOURCE_BINDING_TIER_3 ||
           shaderModelData.HighestShaderModel < D3D_SHADER_MODEL_6_6)
        {
            spdlog::critical("D3D12 HLSL dynamic resources not supported");
            return false;
        }
    }

    { // check for ray tracing support
        D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};
        if(SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData,
                                                  sizeof(featureSupportData))))
        {
            spdlog::info("{}", featureSupportData.RaytracingTier);
        }

        mRaytracingSupported = featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
    }

    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned
        // will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if(FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        mRootSignatureVersion = featureData.HighestVersion;

        spdlog::info("{}", featureData.HighestVersion);
    }

    collectFormatSupport();

    return true;
}

void DeviceContext::collectFormatSupport()
{
    auto checkFormat = [&](uint32_t formatIndex) {
        D3D12_FEATURE_DATA_FORMAT_SUPPORT featureSupport = {};
        featureSupport.Format = static_cast<DXGI_FORMAT>(formatIndex);

        if(FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &featureSupport, sizeof(featureSupport))))
        {
            return;
        }

        if(featureSupport.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET)
        {
            mRenderTargetFormatSupport.set(formatIndex);
        }
        if(featureSupport.Support1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL)
        {
            mDepthStencilFormatSupport.set(formatIndex);
        }
        if(featureSupport.Support1 & D3D12_FORMAT_SUPPORT1_IA_INDEX_BUFFER) { mIndexFormatSupport.set(formatIndex); }
    };

    for(uint32_t formatIndex = 0; formatIndex <= 115; ++formatIndex)
    {
        checkFormat(formatIndex);
    }

    for(uint32_t formatIndex = 130; formatIndex <= 132; ++formatIndex)
    {
        checkFormat(formatIndex);
    }

    for(uint32_t formatIndex = 189; formatIndex <= 190; ++formatIndex)
    {
        checkFormat(formatIndex);
    }
}
} // namespace d3d12
} // namespace scrap