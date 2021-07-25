#pragma once

#include "RenderDefs.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12FrameCodes.h"

#include <optional>

#include <dxgiformat.h>
#include <gpufmt/format.h>
#include <nonstd/span.hpp>
#include <wrl/client.h>

struct D3D12_INDEX_BUFFER_VIEW;
struct ID3D12Resource;

namespace scrap::d3d12
{
class Buffer
{
public:
    enum class Error
    {
        InvalidFormat,
        FailedToCreateGpuResource,
        FailedToCreateUploadResource,
        InsufficientDescriptorHeapSpace
    };

    struct CommonParams
    {
        ResourceAccessFlags accessFlags;
        std::string_view name;
    };

    struct ElementBased
    {
        uint32_t numElements = 0;
    };

    struct ByteBased
    {
        uint32_t byteSize = 0;
    };

    struct FormattedSpecificParams
    {
        gpufmt::Format format;
    };

    struct StructuredSpecificParams
    {
        uint32_t elementByteSize = 0;
    };

    struct SimpleParams : CommonParams, ByteBased
    {};
    struct FormattedParams : CommonParams, ElementBased, FormattedSpecificParams
    {};
    struct StructuredParams : CommonParams, ElementBased, StructuredSpecificParams
    {};

    Buffer() = default;
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    ~Buffer();

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    std::optional<Error> init(const SimpleParams& params, nonstd::span<std::byte> buffer = {});
    std::optional<Error> init(const FormattedParams& params, nonstd::span<std::byte> buffer = {});
    std::optional<Error> init(const StructuredParams& params, nonstd::span<std::byte> buffer = {});

    ID3D12Resource* getResource() const { return mResource.Get(); }

    D3D12_CPU_DESCRIPTOR_HANDLE getSrvCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getSrvGpu() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getUavCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getUavGpu() const;

    D3D12_INDEX_BUFFER_VIEW getIndexView() const;
    D3D12_INDEX_BUFFER_VIEW getIndexView(DXGI_FORMAT format) const;

    bool isReady() const;
    void markAsUsed();

private:
    enum class Type
    {
        Simple,
        Formatted,
        Structured
    };

    struct Params : CommonParams, ByteBased, ElementBased, FormattedSpecificParams, StructuredSpecificParams
    {
        Params() = default;
        Params(const SimpleParams& params): CommonParams(params), ByteBased(params), type(Type::Simple) {}

        Params(const FormattedParams& params)
            : CommonParams(params)
            , ElementBased(params)
            , FormattedSpecificParams(params)
            , type(Type::Formatted)
        {}

        Params(const StructuredParams& params)
            : CommonParams(params)
            , ElementBased(params)
            , StructuredSpecificParams(params)
            , type(Type::Structured)
        {}

        DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
        Type type;
    };

    std::optional<Error> initInternal(Params params, nonstd::span<std::byte> buffer);

    Params mParams;
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    FixedDescriptorHeapReservation mDescriptorHeapReservation;
    uint32_t mSrvIndex = 0;
    uint32_t mUavIndex = 0;

    CopyFrameCode mUploadFrameCode;
    RenderFrameCode mLastUsedFrameCode;
};
} // namespace scrap::d3d12