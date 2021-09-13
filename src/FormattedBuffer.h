#pragma once

#include <gpufmt/format.h>
#include <nonstd/span.hpp>

namespace scrap
{
struct FormattedBufferSpan
{
    gpufmt::Format format;
    nonstd::span<std::byte> buffer;

    template<class T>
    [[nodiscard]] nonstd::span<T> accessBufferAs()
    {
        return nonstd::span<T>(reinterpret_cast<T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
    }

    template<class T>
    [[nodiscard]] nonstd::span<const T> getBufferAs() const
    {
        return nonstd::span<const T>(reinterpret_cast<const T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
    }

    [[nodiscard]] size_t elementCount() const;

    [[nodiscard]] nonstd::span<std::byte> accessElement(size_t index);
    [[nodiscard]] nonstd::span<const std::byte> getElement(size_t index) const;

    template<class T>
    [[nodiscard]] T& accessElementAs(size_t index)
    {
        nonstd::span elementSpan = accessElement(index);

        if(elementSpan.size_bytes() != sizeof(T)) { return {}; }

        return *reinterpret_cast<T*>(elementSpan.data());
    }

    template<class T>
    [[nodiscard]] const T& getElementAs(size_t index) const
    {
        nonstd::span elementSpan = getElement(index);

        if(elementSpan.size_bytes() != sizeof(T)) { return {}; }

        return *reinterpret_cast<const T*>(elementSpan.data());
    }

    [[nodiscard]] FormattedBufferSpan formattedSubspan(
        size_t elementOffset,
        size_t elementCount = static_cast<nonstd::span<std::byte>::size_type>(nonstd::dynamic_extent)) const;
};

struct FormattedBufferView
{
    gpufmt::Format format;
    nonstd::span<const std::byte> buffer;

    template<class T>
    [[nodiscard]] nonstd::span<const T> getBufferAs() const
    {
        return nonstd::span<const T>(reinterpret_cast<const T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
    }

    [[nodiscard]] size_t elementCount() const;

    [[nodiscard]] nonstd::span<const std::byte> getElement(size_t index) const;

    template<class T>
    [[nodiscard]] const T& getElementAs(size_t index) const
    {
        nonstd::span elementSpan = getElement(index);

        if(elementSpan.size_bytes() != sizeof(T)) { return {}; }

        return *reinterpret_cast<const T*>(elementSpan.data());
    }

    [[nodiscard]] FormattedBufferView formattedSubview(
        size_t elementOffset,
        size_t elementCount = static_cast<nonstd::span<std::byte>::size_type>(nonstd::dynamic_extent)) const;
};
} // namespace scrap