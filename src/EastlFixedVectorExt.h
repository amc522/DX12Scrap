#pragma once

#include <span>

#include <EASTL/fixed_vector.h>

namespace scrap
{
template<class T, size_t N, bool EnableOverflow, class OverflowAllocator>
[[nodiscard]] std::span<T, N> ToSpan(eastl::fixed_vector<T, N, EnableOverflow, OverflowAllocator>& vec) noexcept
{
    return std::span<T, N>(vec.data(), N);
}

template<class T, size_t N, bool EnableOverflow, class OverflowAllocator>
[[nodiscard]] std::span<const T, N>
ToSpan(const eastl::fixed_vector<T, N, EnableOverflow, OverflowAllocator>& vec) noexcept
{
    return std::span<const T, N>(vec.data(), N);
}
} // namespace scrap