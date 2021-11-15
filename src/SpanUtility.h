#pragma once

#include <nonstd/span.hpp>

namespace scrap
{
template<class T>
[[nodiscard]] constexpr nonstd::span<const std::byte> ToByteSpan(const T& value) noexcept
{
    return nonstd::as_bytes(nonstd::span(&value, 1));
}

template<class T>
[[nodiscard]] constexpr nonstd::span<std::byte> ToWritableByteSpan(T& value) noexcept
{
    return nonstd::as_writable_bytes(nonstd::span(&value, 1));
}
} // namespace scrap