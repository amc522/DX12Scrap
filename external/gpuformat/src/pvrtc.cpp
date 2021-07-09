#include "gpufmt/internal/pvrtc.h"

#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS)

#include "gpufmt/definitions.h"

#include <PowerVR/PVRTDecompress.h>

#include <array>

namespace gpufmt::internal {
#if defined(GF_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4701) //potentially uninitialized local variable 'finalTexels' used
#endif

    bool decompressPVRTCBlock_2BPP(const gpufmt::Surface<const uint64_t> &compressedBlocks, const gpufmt::Surface<glm::u8vec4> decompressedTexels) noexcept {
        return pvr::PVRTDecompressPVRTC(compressedBlocks.blockData.data(),
                                        1,
                                        compressedBlocks.extentInBlocks.x * 8,
                                        compressedBlocks.extentInBlocks.y * 4,
                                        reinterpret_cast<uint8_t*>(decompressedTexels.blockData.data())) != 0;
    }
    
    bool decompressPVRTCBlock_4BPP(const gpufmt::Surface<const uint64_t> &compressedBlocks, const gpufmt::Surface<glm::u8vec4> decompressedTexels) noexcept {
        return pvr::PVRTDecompressPVRTC(compressedBlocks.blockData.data(),
                                        0,
                                        compressedBlocks.extentInBlocks.x * 4,
                                        compressedBlocks.extentInBlocks.y * 4,
                                        reinterpret_cast<uint8_t *>(decompressedTexels.blockData.data())) != 0;
    }

#if defined(GF_COMPILER_MSVC)
#pragma warning(pop)
#endif
}

#endif