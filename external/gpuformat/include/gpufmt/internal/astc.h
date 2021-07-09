#pragma once

#include <gpufmt/config.h>

#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_ASTC_COMPRESSED_FORMATS)

#include <cstdint>

namespace gpufmt::internal {
    bool decompressASTCBlock(gpufmt::span<gpufmt::byte> dest, gpufmt::span<const gpufmt::byte> source, bool isSRGB, int blockWidth, int blockHeight);
}

#endif