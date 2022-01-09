#pragma once

namespace scrap::d3d12
{
template<class T>
class GpuWriteGuard
{
public:
    GpuWriteGuard() = default;
    GpuWriteGuard(T& gpuBuffer, ID3D12GraphicsCommandList* commandList)
        : mGpuBuffer(&gpuBuffer)
        , mCommandList(commandList)
    {
        mWriteBuffer = mGpuBuffer->map();
    }

    GpuWriteGuard(const GpuWriteGuard<T>&) = delete;
    GpuWriteGuard(GpuWriteGuard<T>&& other)
        : mGpuBuffer(other.mGpuBuffer)
        , mCommandList(other.mCommandList)
        , mWriteBuffer(std::move(other.mWriteBuffer))
    {
        other.mGpuBuffer = nullptr;
        other.mCommandList = nullptr;
    }

    ~GpuWriteGuard()
    {
        if(mGpuBuffer != nullptr) { mGpuBuffer->unmap(mCommandList); }
    }

    GpuWriteGuard& operator=(const GpuWriteGuard<T>&) = delete;
    GpuWriteGuard& operator=(GpuWriteGuard<T>&& other)
    {
        if(mGpuBuffer != nullptr) { mGpuBuffer->unmap(mCommandList); }
        mGpuBuffer = other.mGpuBuffer;
        other.mGpuBuffer = nullptr;

        mCommandList = other.mCommandList;
        other.mCommandList = nullptr;

        mWriteBuffer = std::move(other.mWriteBuffer);

        return *this;
    }

    std::span<std::byte> getWriteBuffer() { return mWriteBuffer; }

    template<class U>
    std::span<U> getWriteBufferAs()
    {
        return std::span<U>(reinterpret_cast<U*>(mWriteBuffer.data()), mWriteBuffer.size_bytes() / sizeof(U));
    }

private:
    T* mGpuBuffer = nullptr;
    ID3D12GraphicsCommandList* mCommandList = nullptr;
    std::span<std::byte> mWriteBuffer;
};
} // namespace scrap::d3d12