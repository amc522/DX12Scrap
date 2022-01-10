#pragma once

#include "FreeBlockTracker.h"
#include "d3d12/D3D12Fwd.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <span>

#include <wrl/client.h>

namespace scrap::d3d12
{
struct UploadBufferMap
{
    std::span<std::byte> writeBuffer;
    size_t bufferIndex = 0;
    size_t byteOffset = 0;
};

struct UploadBufferCopyData
{
    ID3D12Resource* buffer = nullptr;
    size_t byteSize = 0;
    size_t byteOffset = 0;
};

class UploadBufferPool
{
public:
    void init();

    void update();

    UploadBufferMap map(size_t bytes);
    UploadBufferCopyData unmap(const UploadBufferMap& bufferMap);
    
private:
    constexpr static size_t sMinBufferByteSize = 1024 * 1024; // 1MB

    UploadBufferMap mapUnchecked(size_t index, size_t byteSize);

    void pushBackBuffer(size_t byteSize = sMinBufferByteSize);

    struct UploadBuffer
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
        size_t byteSize = 0;
        size_t byteOffset = 0;
    };
    std::vector<UploadBuffer> mUploadBuffers;
};
} // namespace scrap::d3d12