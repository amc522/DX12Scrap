#pragma once

#include <array>
#include <version>

#ifdef _MSVC_LANG
#define GF_CPP_VERSION _MSVC_LANG
#else
#define GF_CPP_VERSION __cplusplus
#endif

#if defined(__cpp_lib_span) && GF_CPP_VERSION >= __cpp_lib_span
#include <span>
#define GF_HAS_STD_SPAN
#elif __has_include("span.hpp")
#include <span.hpp>
#elif __has_include("nonstd/span.hpp")
#include <nonstd/span.hpp>
#endif

// For glm::tvec3 used as Extent. If redefining gpufmt::Extent to another type, you can remove this include.
#include <glm/vec3.hpp> 

namespace gpufmt {
#ifdef GF_HAS_STD_SPAN
    inline constexpr auto span_dynamic_extent = std::dynamic_extent;

    template<class T, size_t Extent = std::dynamic_extent>
    using span = std::span<T, Extent>;
#elif defined(TCB_SPAN_HPP_INCLUDED)
    inline constexpr auto span_dynamic_extent = tcb::dynamic_extent;

    template<class T, size_t Extent = tcb::dynamic_extent>
    using span = tcp::span<T, Extent>;
#elif defined(NONSTD_SPAN_HPP_INCLUDED)
    inline constexpr auto span_dynamic_extent = nonstd::dynamic_extent;

    template<class T, size_t Extent = nonstd::dynamic_extent>
    using span = std::span<T, Extent>;
#endif

    using Extent = glm::tvec3<uint32_t>;
    using byte = std::byte;

    template<class T>
    gpufmt::span<const gpufmt::byte> asBytes(const T &value) {
        return gpufmt::span<const gpufmt::byte>{reinterpret_cast<const std::byte*>(&value), sizeof(T)};
    }

    template<class T>
    gpufmt::span<gpufmt::byte> asWritableBytes(T &value) {
        return gpufmt::span<gpufmt::byte>{reinterpret_cast<std::byte*>(&value), sizeof(T)};
    }

    template<class T, size_t Size>
    gpufmt::span<const gpufmt::byte> asBytes(const std::array<T, Size> &arr) {
        return gpufmt::span<const gpufmt::byte>(reinterpret_cast<const gpufmt::byte*>(arr.data()), arr.size() * sizeof(T));
    }

    template<class T, size_t Size>
    gpufmt::span<gpufmt::byte> asWritableBytes(std::array<T, Size> &arr) {
        return gpufmt::span<gpufmt::byte>(reinterpret_cast<gpufmt::byte*>(arr.data()), arr.size() * sizeof(T));
    }

    template<class T>
    gpufmt::span<const gpufmt::byte> asBytes(gpufmt::span<const T> arr) {
        return gpufmt::span<const gpufmt::byte>(reinterpret_cast<const gpufmt::byte*>(arr.data()), arr.size_bytes());
    }

    template<class T>
    gpufmt::span<gpufmt::byte> asWritableBytes(gpufmt::span<T> arr) {
        return gpufmt::span<gpufmt::byte>(reinterpret_cast<gpufmt::byte*>(arr.data()), arr.size_bytes());
    }
}

//#define GF_EXCLUDE_64BIT_FORMATS
//#define GF_EXCLUDE_SCALED_FORMATS
//#define GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
//#define GF_EXCLUDE_BC_COMPRESSED_FORMATS
//#define GF_EXCLUDE_ETC_COMPRESSED_FORMATS
//#define GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
//#define GF_EXCLUDE_COMPRESSED_FORMATS