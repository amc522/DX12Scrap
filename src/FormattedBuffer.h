#pragma once

#include <span>

#include <gpufmt/format.h>

namespace scrap
{
struct FormattedBufferSpan
{
    gpufmt::Format format;
    std::span<std::byte> buffer;

    template<class T>
    [[nodiscard]] std::span<T> accessBufferAs()
    {
        return std::span<T>(reinterpret_cast<T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
    }

    template<class T>
    [[nodiscard]] std::span<const T> getBufferAs() const
    {
        return std::span<const T>(reinterpret_cast<const T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
    }

    [[nodiscard]] size_t elementCount() const;

    [[nodiscard]] std::span<std::byte> accessElement(size_t index);
    [[nodiscard]] std::span<const std::byte> getElement(size_t index) const;

    template<class T>
    [[nodiscard]] T& accessElementAs(size_t index)
    {
        std::span elementSpan = accessElement(index);

        if(elementSpan.size_bytes() != sizeof(T)) { return {}; }

        return *reinterpret_cast<T*>(elementSpan.data());
    }

    template<class T>
    [[nodiscard]] const T& getElementAs(size_t index) const
    {
        std::span elementSpan = getElement(index);

        if(elementSpan.size_bytes() != sizeof(T)) { return {}; }

        return *reinterpret_cast<const T*>(elementSpan.data());
    }

    [[nodiscard]] FormattedBufferSpan
    formattedSubspan(size_t elementOffset,
                     size_t elementCount = static_cast<std::span<std::byte>::size_type>(std::dynamic_extent)) const;
};

struct FormattedBufferView
{
    gpufmt::Format format;
    std::span<const std::byte> buffer;

    template<class T>
    [[nodiscard]] std::span<const T> getBufferAs() const
    {
        return std::span<const T>(reinterpret_cast<const T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
    }

    [[nodiscard]] size_t elementCount() const;

    [[nodiscard]] std::span<const std::byte> getElement(size_t index) const;

    template<class T>
    [[nodiscard]] const T& getElementAs(size_t index) const
    {
        std::span elementSpan = getElement(index);

        if(elementSpan.size_bytes() != sizeof(T)) { return {}; }

        return *reinterpret_cast<const T*>(elementSpan.data());
    }

    [[nodiscard]] FormattedBufferView
    formattedSubview(size_t elementOffset,
                     size_t elementCount = static_cast<std::span<std::byte>::size_type>(std::dynamic_extent)) const;
};
} // namespace scrap