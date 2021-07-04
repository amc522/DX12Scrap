#include "RenderScene.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap
{
RenderScene::RenderScene(d3d12::DeviceContext& d3d12Context)
{
    // Create an empty root signature.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned
        // will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if(FAILED(d3d12Context.getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData,
                                                                sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_range1
        // This describes the descriptors that will be available to the shader. A descriptor in this case being an SRV,
        // UAV, or CBV. For this simple setup, we have a descriptor range saying it has 1 descriptor (SRV), our texture.
        std::array<D3D12_DESCRIPTOR_RANGE1, 1> ranges = {};
        D3D12_DESCRIPTOR_RANGE1& range = ranges[0];
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range.NumDescriptors = 1;
        range.BaseShaderRegister = 0;
        range.OffsetInDescriptorsFromTableStart = 0;
        range.RegisterSpace = 0;
        range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_root_parameter1
        // The root parameter describes a parameter or argument being passed into the shader as described or declared by
        // the root signautre. Here we are saying we have a descriptor table (a list of descritor ranges), with one
        // range in it.
        std::array<D3D12_ROOT_PARAMETER1, 1> rootParameters = {};
        D3D12_ROOT_PARAMETER1& rootParameter = rootParameters[0];
        rootParameter.DescriptorTable.NumDescriptorRanges = 1;
        rootParameter.DescriptorTable.pDescriptorRanges = ranges.data();
        rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_static_sampler_desc
        // Static samplers are the same as normal samplers, except that they are not going to change after creating the
        // root signature. Static samplers can be used to describe all the basic samplers that are used by most shaders
        // in an engine; point, bilinear, trilinear, anisotropic 1-16, etc. Probably only some samplers used for shadows
        // will need to be dynammic. There can be an unlimited number of static samplers in the root signature at no
        // cost.
        std::array<D3D12_STATIC_SAMPLER_DESC, 1> staticSamplers = {};
        D3D12_STATIC_SAMPLER_DESC& sampler = staticSamplers[0];
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_versioned_root_signature_desc
        D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        rootSignatureDesc.Desc_1_1.NumParameters = (UINT)rootParameters.size();
        rootSignatureDesc.Desc_1_1.pParameters = rootParameters.data();
        rootSignatureDesc.Desc_1_1.NumStaticSamplers = (UINT)staticSamplers.size();
        rootSignatureDesc.Desc_1_1.pStaticSamplers = staticSamplers.data();
        rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        HRESULT hr =
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
        if(FAILED(hr))
        {
            if(error != nullptr)
            {
                spdlog::critical("Failed to serialize d3d12 root signature. {}",
                                 std::string_view((const CHAR*)error->GetBufferPointer(), error->GetBufferSize()));
            }
            else
            {
                spdlog::critical("Failed to serialize d3d12 root signature");
            }
            return;
        }

        hr = d3d12Context.getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                                           IID_PPV_ARGS(&mRootSignature));

        if(FAILED(hr))
        {
            spdlog::critical("Failed to create d3d12 root signature");
            return;
        }

        spdlog::info("Created d3d12 root signature");
    }

    if(!loadShaders(d3d12Context.getDevice())) { return; }

    { // Create the command list.
        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandlist
        HRESULT hr = d3d12Context.getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                                 d3d12Context.getCommandAllocator(), mPso.Get(),
                                                                 IID_PPV_ARGS(&mCommandList));

        if(FAILED(hr))
        {
            spdlog::critical("Failed to create d3d12 command list");
            return;
        }

        spdlog::info("Created d3d12 command list");
    }

    // Note: ComPtr's are CPU objects but this resource needs to stay in scope until
    // the command list that references it has finished executing on the GPU.
    // We will flush the GPU at the end of this method to ensure the resource is not
    // prematurely destroyed.
    ComPtr<ID3D12Resource> textureUploadHeap;

    { // Create the texture.
        // 1. Create GPU texture
        //    a. Get allocation info. This will get the byte aligment for the texture. You can set the alignment in
        //       D3D12_RESOURCE_DESC to 0 and it will figure out the alignment. Expicitly grabbing it here to use later.
        // 2. Create upload buffer
        // 3. Copy cpu texture data to upload buffer
        //    aa. Get some kind of texture data
        //    a. Query for the actual hardware storage information of the texture (D3D12_PLACED_SUBRESOURCE_FOOTPRINT
        //       via GetCopyableFootprints)
        //    b. Acquire pointer to upload texture memory (via Map)
        //    c. Copy cpu texture data to mapped upload buffer data
        //    d. Signal to the upload texture that the memory copy is finished (via Unmap)
        // 4. Copy the upload buffer to the final GPU texture (via CopyTextureRegion)
        // 5. Transition the resource from the copy state to the shader resource state
        // 6. Create the SRV

        // Q: Why use two different GPU textures?
        // A: GPUs have different kinds of memory that are made faster for certain tasks but are slower for others. The
        // "upload" memory if more efficient at reading from cpu memory, but has less bandwidth available for the gpu
        // processor. The memory where the texture eventually ends up can't read from the cpu, and has full bandwidth to
        // the gpu processor. This type of memory is most of the memory on the gpu.
        // For more information read https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_type.
        // Specifically look at the descriptions for 'D3D12_HEAP_TYPE_UPLOAD' and 'D3D12_HEAP_TYPE_DEFAULT'.

        constexpr UINT textureWidth = 1024;
        constexpr UINT textureHeight = 1024;
        constexpr UINT bytesPerPixel = 4;

        D3D12_RESOURCE_ALLOCATION_INFO allocInfo;

        //=======================
        // 1. Create GPU texture
        //=======================
        D3D12_RESOURCE_DESC textureDesc = {};
        {
            // Describe a Texture2D.
            textureDesc.MipLevels = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.Width = textureWidth;
            textureDesc.Height = textureHeight;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            textureDesc.DepthOrArraySize = 1;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            //-----------------------------------------------------------------------------------
            // 1.a.
            // Get the alignment and size of the resource that will actually be used on the gpu.
            // ----------------------------------------------------------------------------------

            // Here's and article describing in detail what this memory alignment stuff is all about:
            // https://asawicki.info/news_1726_secrets_of_direct3d_12_resource_alignment

            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-getresourceallocationinfo
            allocInfo = d3d12Context.getDevice()->GetResourceAllocationInfo(0, 1, &textureDesc);
            textureDesc.Alignment = allocInfo.Alignment;

            // The destination heap of the texture. This is the heap where the texture will be for its lifetime on the
            // gpu.
            D3D12_HEAP_PROPERTIES defaultHeapProps = {};
            defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            defaultHeapProps.CreationNodeMask = 0;
            defaultHeapProps.VisibleNodeMask = 0;

            // Actually create the resource
            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommittedresource
            HRESULT hr = d3d12Context.getDevice()->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE,
                                                                           &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST,
                                                                           nullptr, IID_PPV_ARGS(&mTexture));

            if(FAILED(hr))
            {
                spdlog::critical("Failed to create d3d12 gpu texture");
                return;
            }

            mTexture->SetName(L"FirstTexture");
        }

        //==========================
        // 2. Create upload buffer
        //==========================
        D3D12_RESOURCE_DESC textureUploadDesc = {};
        {
            textureUploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            textureUploadDesc.Alignment = allocInfo.Alignment;
            textureUploadDesc.Width = allocInfo.SizeInBytes;
            textureUploadDesc.Height = 1;
            textureUploadDesc.DepthOrArraySize = 1;
            textureUploadDesc.MipLevels = 1;
            textureUploadDesc.Format = DXGI_FORMAT_UNKNOWN;
            textureUploadDesc.SampleDesc.Count = 1;
            textureUploadDesc.SampleDesc.Quality = 0;
            textureUploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            textureUploadDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            D3D12_HEAP_PROPERTIES uploadHeapProps = {};
            uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            uploadHeapProps.CreationNodeMask = 0;
            uploadHeapProps.VisibleNodeMask = 0;

            HRESULT hr = d3d12Context.getDevice()->CreateCommittedResource(
                &uploadHeapProps, D3D12_HEAP_FLAG_NONE, &textureUploadDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                IID_PPV_ARGS(&textureUploadHeap));
            if(FAILED(hr))
            {
                spdlog::critical("Failed to create texture upload resource.");
                return;
            }

            textureUploadHeap->SetName(L"FirstTextureUpload");
        }

        //============================================
        // 3. Copy cpu texture data to upload buffer
        //============================================
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT subresourceFootprint;
        {
            //-------------------------------------
            // 3.aa. Generate a checkboard texture
            //-------------------------------------

            std::vector<UINT8> texture = [&]() {
                const UINT rowPitch = textureWidth * bytesPerPixel;
                const UINT cellPitch = rowPitch >> 3;      // The width of a cell in the checkboard texture.
                const UINT cellHeight = textureWidth >> 3; // The height of a cell in the checkerboard texture.
                const UINT textureSize = rowPitch * textureHeight;

                std::vector<UINT8> data(textureSize);
                UINT8* pData = &data[0];

                for(UINT n = 0; n < textureSize; n += bytesPerPixel)
                {
                    UINT x = n % rowPitch;
                    UINT y = n / rowPitch;
                    UINT i = x / cellPitch;
                    UINT j = y / cellHeight;

                    if(i % 2 == j % 2)
                    {
                        pData[n] = 0x00;     // R
                        pData[n + 1] = 0x00; // G
                        pData[n + 2] = 0x00; // B
                        pData[n + 3] = 0xff; // A
                    }
                    else
                    {
                        pData[n] = 0xff;     // R
                        pData[n + 1] = 0xff; // G
                        pData[n + 2] = 0xff; // B
                        pData[n + 3] = 0xff; // A
                    }
                }

                return data;
            }();

            D3D12_SUBRESOURCE_DATA textureData = {};
            textureData.pData = texture.data();
            textureData.RowPitch = textureWidth * bytesPerPixel;
            textureData.SlicePitch = textureData.RowPitch * textureHeight;

            //----------------------------------------------------------------------------------------------------------
            // 3.a. Query for the actual hardware storage information of the texture (D3D12_PLACED_SUBRESOURCE_FOOTPRINT
            //      via GetCopyableFootprints)
            //----------------------------------------------------------------------------------------------------------

            UINT64 rowByteSize;
            UINT numRows;
            UINT64 totalBytes;

            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-getcopyablefootprints
            d3d12Context.getDevice()->GetCopyableFootprints(
                &textureDesc,
                0,                     // first subresource
                1,                     // num subresources
                0,                     // base offset
                &subresourceFootprint, // (out) D3D12_PLACED_SUBRESOURCE_FOOTPRINT array
                &numRows,              // (out) total bytes
                &rowByteSize,          // (out) row byte size
                &totalBytes            // (out) totalBytes
            );

            if(textureUploadDesc.Width < totalBytes + subresourceFootprint.Offset)
            {
                spdlog::critical("Texture upload heap is too small");
                return;
            }

            //---------------------------------------------------------
            // 3.b. Acquire pointer to upload texture memory (via Map)
            //---------------------------------------------------------
            BYTE* pMapData;
            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12resource-map
            HRESULT hr = textureUploadHeap->Map(0, nullptr, reinterpret_cast<void**>(&pMapData));
            if(FAILED(hr))
            {
                spdlog::critical("Failed to map texture upload heap to a cpu buffer");
                return;
            }

            //----------------------------------------------------------
            // 3.c. Copy cpu texture data to mapped upload texture data
            //----------------------------------------------------------
            D3D12_MEMCPY_DEST destData = {};
            destData.pData = pMapData + subresourceFootprint.Offset;
            destData.RowPitch = subresourceFootprint.Footprint.RowPitch;
            destData.SlicePitch = SIZE_T(subresourceFootprint.Footprint.RowPitch) * SIZE_T(numRows);

            // d3d12x function
            MemcpySubresource(&destData,                           // destination
                              &textureData,                        // source
                              static_cast<SIZE_T>(rowByteSize),    // row size in bytes
                              numRows,                             // number of rows
                              subresourceFootprint.Footprint.Depth // number of slices
            );

            //--------------------------------------------------------------------------------
            // 3.d. Signal to the upload texture that the memory copy is finished (via Unmap)
            //--------------------------------------------------------------------------------

            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12resource-unmap
            textureUploadHeap->Unmap(0, nullptr);
        }

        //=============================================================================
        // 4. Copy the upload buffer to the final GPU texture (via CopyTextureRegion)
        //=============================================================================
        {
            D3D12_TEXTURE_COPY_LOCATION destCopyLocation = {};
            destCopyLocation.pResource = mTexture.Get();
            destCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            destCopyLocation.SubresourceIndex = 0;

            D3D12_TEXTURE_COPY_LOCATION sourceCopyLocation;
            sourceCopyLocation.pResource = textureUploadHeap.Get();
            sourceCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            sourceCopyLocation.PlacedFootprint = subresourceFootprint;

            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist-copytextureregion
            mCommandList->CopyTextureRegion(&destCopyLocation, 0, 0, 0, &sourceCopyLocation, nullptr);
        }

        //=============================================================================
        // 5. Transition the resource from the copy state to the shader resource state
        //=============================================================================
        {
            D3D12_RESOURCE_BARRIER transitionBarrier = {};
            transitionBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            transitionBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            transitionBarrier.Transition.pResource = mTexture.Get();
            transitionBarrier.Transition.Subresource = 0;
            transitionBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            transitionBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

            // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist-resourcebarrier
            mCommandList->ResourceBarrier(1, &transitionBarrier);
        }

        //===================
        // 6. Create the SRV
        //===================
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Texture2D.MipLevels = 1;

            auto descriptor =
                d3d12Context.getCbvSrvUavHeap().createShaderResourceView(d3d12Context, mTexture.Get(), srvDesc);
            if(!descriptor)
            {
                spdlog::critical("Failed to create texture SRV");
                return;
            }

            mTextureSrv = std::move(descriptor.value());
        }
    }

    if(FAILED(mCommandList->Close())) { spdlog::error("Failed to close graphics command list"); }

    // Execute the command list.
    std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.Get()};
    d3d12Context.getCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());

    { // Create synchronization objects and wait until assets have been uploaded to the GPU.
        uint32_t currentFrameIndex = d3d12Context.getFrameIndex();
        HRESULT hr = d3d12Context.getDevice()->CreateFence(mFenceValues[currentFrameIndex], D3D12_FENCE_FLAG_NONE,
                                                           IID_PPV_ARGS(&mFence));
        ++mFenceValues[currentFrameIndex];

        // Create an event handle to use for frame synchronization.
        // https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventw
        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if(mFenceEvent == nullptr)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            spdlog::critical("Failed to create fence event");
        }

        // Wait for the command list to execute; we are reusing the same command
        // list in our main loop but for now, we just want to wait for setup to
        // complete before continuing.
        waitOnGpu(d3d12Context);

        spdlog::info("Created syncronization objects");
    }

    mInitialized = true;
}

RenderScene::~RenderScene()
{
    spdlog::info("Destroying RenderScene");
    if(mFenceEvent != nullptr) { CloseHandle(mFenceEvent); }
}

bool RenderScene::loadShaders(ID3D12Device* device)
{
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ComPtr<ID3DBlob> error;
    // Shader paths are hardcoded and only work with the directory structure of the project
    constexpr std::wstring_view vertexShaderPath = L"assets\\hello_triangle.hlsl";
    constexpr std::wstring_view pixelShaderPath = L"assets\\hello_triangle.hlsl";

    // https://docs.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompilefromfile
    HRESULT hr = D3DCompileFromFile(vertexShaderPath.data(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0,
                                    &vertexShader, &error);
    if(FAILED(hr))
    {
        if(error != nullptr)
        {
            spdlog::critical("Failed to compile vertex shader.\n{}",
                             std::string_view((const char*)error->GetBufferPointer(), error->GetBufferSize()));
        }
        else
        {
            spdlog::critical(L"Failed to compile vertex shader {}", vertexShaderPath);
        }

        return false;
    }

    spdlog::info(L"Compiled vertex shader {}", vertexShaderPath);

    hr = D3DCompileFromFile(pixelShaderPath.data(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader,
                            &error);

    if(FAILED(hr))
    {
        if(error != nullptr)
        {
            spdlog::critical("Failed to compile pixel shader.\n{}",
                             std::string_view((const char*)error->GetBufferPointer(), error->GetBufferSize()));
        }
        else
        {
            spdlog::critical(L"Failed to compile pixel shader {}", pixelShaderPath);
        }

        return false;
    }

    spdlog::info(L"Compiled pixel shader {}", pixelShaderPath);

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-creategraphicspipelinestate
    if(FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPso))))
    {
        spdlog::critical("Failed to create d3d12 pipeline state object");
        return false;
    }

    return true;
}

void RenderScene::render(d3d12::DeviceContext& d3d12Context)
{
    // Command list allocators can only be reset when the associated
    // command lists have finished execution on the GPU; apps should use
    // fences to determine GPU execution progress.
    if(FAILED(d3d12Context.getCommandAllocator()->Reset()))
    {
        spdlog::error("Failed to reset d3d12 command allocator");
        return;
    }

    // However, when ExecuteCommandList() is called on a particular command
    // list, that command list can then be reset at any time and must be before
    // re-recording.
    if(FAILED(mCommandList->Reset(d3d12Context.getCommandAllocator(), mPso.Get())))
    {
        spdlog::error("Failed to reset d3d12 command list");
        return;
    }

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(d3d12Context.frameSize().x);
    viewport.Height = static_cast<float>(d3d12Context.frameSize().y);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissorRect{static_cast<LONG>(viewport.TopLeftX), static_cast<LONG>(viewport.TopLeftY),
                           static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height)};

    // Set necessary state.
    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

    std::array<ID3D12DescriptorHeap*, 1> descriptorHeaps = {d3d12Context.getCbvSrvUavHeap().getGpuDescriptorHeap()};
    mCommandList->SetDescriptorHeaps((UINT)descriptorHeaps.size(), descriptorHeaps.data());
    mCommandList->SetGraphicsRootDescriptorTable(
        0, d3d12Context.getCbvSrvUavHeap().getGpuDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    mCommandList->RSSetViewports(1, &viewport);
    mCommandList->RSSetScissorRects(1, &scissorRect);

    // Indicate that the back buffer will be used as a render target.
    D3D12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    mCommandList->ResourceBarrier(1, &renderTargetBarrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12Context.getBackBufferRtv();
    mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
    mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mCommandList->DrawInstanced(3, 1, 0, 0);

    // Indicate that the back buffer will now be used to present.
    renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    mCommandList->ResourceBarrier(1, &renderTargetBarrier);

    if(FAILED(mCommandList->Close()))
    {
        spdlog::error("Failed to close d3d12 command list");
        return;
    }

    // Execute the command list.
    std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.Get()};
    d3d12Context.getCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());

    // Present the frame.
    d3d12Context.present();

    endFrame(d3d12Context);
}

void RenderScene::shutdown(d3d12::DeviceContext& d3d12Context)
{
    spdlog::info("Shutting down RenderScene");
    waitOnGpu(d3d12Context);
}

void RenderScene::waitOnGpu(d3d12::DeviceContext& d3d12Context)
{
    if(mFence == nullptr) { return; }

    const uint64_t fenceValue = mFenceValues[d3d12Context.getFrameIndex()];

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12commandqueue-signal
    if(FAILED(d3d12Context.getCommandQueue()->Signal(mFence.Get(), fenceValue)))
    {
        spdlog::error("Failed to update fence.");
        return;
    }

    if(FAILED(mFence->SetEventOnCompletion(fenceValue, mFenceEvent)))
    {
        spdlog::error("Fence SetEventOnCompletion call failed");
        return;
    }

    WaitForSingleObject(mFenceEvent, INFINITE);
}

void RenderScene::endFrame(d3d12::DeviceContext& d3d12Context)
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = mFenceValues[d3d12Context.getFrameIndex()];
    if(FAILED(d3d12Context.getCommandQueue()->Signal(mFence.Get(), currentFenceValue)))
    {
        spdlog::error("Failed to signal command queue;");
        return;
    }

    // Update the frame index.
    d3d12Context.swap();
    const uint32_t currentFrameIndex = d3d12Context.getFrameIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if(mFence->GetCompletedValue() < mFenceValues[currentFrameIndex])
    {
        if(FAILED(mFence->SetEventOnCompletion(mFenceValues[currentFrameIndex], mFenceEvent)))
        {
            spdlog::error("Fence SetEventOnCompletion call failed");
            return;
        }

        WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    mFenceValues[currentFrameIndex] = currentFenceValue + 1;
}

} // namespace scrap
