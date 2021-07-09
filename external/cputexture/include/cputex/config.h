#pragma once

#include <gpufmt/config.h>

namespace cputex {
    using byte = gpufmt::byte;
    template<class T, size_t Extent = gpufmt::span_dynamic_extent>
    using span = gpufmt::span<T, Extent>;
}