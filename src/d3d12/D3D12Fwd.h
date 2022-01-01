#pragma once

namespace scrap::d3d12
{
class BLAccelerationStructure;
class Buffer;
class CopyContext;
class Debug;
class DeviceContext;
class FixedDescriptorHeap;
class GraphicsCommandList;
class GraphicsContext;
class GraphicsPipelineState;
struct GraphicsPipelineStateParams;
class GraphicsShader;
struct GraphicsShaderParams;
class MonotonicDescriptorHeap;
class RaytracingDispatchPipelineState;
class RaytracingPipelineState;
class RaytracingShader;
struct RaytracingShaderParams;
class ShaderTable;
class Texture;
class TLAccelerationStructure;
}

enum D3D_FEATURE_LEVEL;
enum D3D_ROOT_SIGNATURE_VERSION;
enum D3D12_COMMAND_LIST_TYPE;
enum D3D12_DESCRIPTOR_HEAP_TYPE;
struct D3D12_CONSTANT_BUFFER_VIEW_DESC;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_DEPTH_STENCIL_VIEW_DESC;
struct D3D12_DESCRIPTOR_HEAP_DESC;
struct D3D12_GPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_VIRTUAL_ADDRESS_RANGE;
struct D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE;
struct D3D12_RENDER_TARGET_VIEW_DESC;
struct D3D12_SAMPLER_DESC;
struct D3D12_SHADER_BYTECODE;
struct D3D12_SHADER_RESOURCE_VIEW_DESC;
struct D3D12_UNORDERED_ACCESS_VIEW_DESC;
struct ID3D12CommandAllocator;
struct ID3D12CommandList;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12Device;
struct ID3D12Device1;
struct ID3D12Device2;
struct ID3D12Device3;
struct ID3D12Device4;
struct ID3D12Device5;
struct ID3D12Device6;
struct ID3D12DeviceChild;
struct ID3D12Fence;
struct ID3D12GraphicsCommandList;
struct ID3D12GraphicsCommandList4;
struct ID3D12PipelineState;
struct ID3D12Resource;
struct ID3D12ShaderReflection;
struct ID3D12ShaderReflectionConstantBuffer;
struct IDXGIAdapter4;
struct IDXGIFactory4;
struct IDXGISwapChain3;
struct ID3D12RootSignature;
struct ID3D12CommandAllocator;

struct _D3D12_SHADER_INPUT_BIND_DESC;
using D3D12_SHADER_INPUT_BIND_DESC = _D3D12_SHADER_INPUT_BIND_DESC;

struct ID3D10Blob;
using ID3DBlob = ID3D10Blob;

struct _D3D12_SHADER_BUFFER_DESC;
using D3D12_SHADER_BUFFER_DESC = _D3D12_SHADER_BUFFER_DESC;

enum _D3D_SHADER_INPUT_TYPE;
using D3D_SHADER_INPUT_TYPE = _D3D_SHADER_INPUT_TYPE;