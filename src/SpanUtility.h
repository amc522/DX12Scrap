#pragma once

#include <span>

namespace scrap
{
template<class T>
[[nodiscard]] constexpr std::span<const std::byte> ToByteSpan(const T& value) noexcept
{
    return std::as_bytes(std::span(&value, 1));
}

template<class T>
[[nodiscard]] constexpr std::span<std::byte> ToWritableByteSpan(T& value) noexcept
{
    return std::as_writable_bytes(std::span(&value, 1));
}
} // namespace scrap