#include "gpufmt/internal/astc.h"

#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_ASTC_COMPRESSED_FORMATS)

#include <astc_dec/astc_decomp.h>

bool gpufmt::internal::decompressASTCBlock(gpufmt::span<gpufmt::byte> dest, gpufmt::span<const gpufmt::byte> source, bool isSRGB, int blockWidth, int blockHeight) {
    return basisu::astc::decompress(reinterpret_cast<uint8_t*>(dest.data()),
                                    reinterpret_cast<const uint8_t*>(source.data()),
                                    isSRGB, blockWidth, blockHeight);
}

#endif