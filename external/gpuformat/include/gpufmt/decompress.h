#pragma once

#include <gpufmt/config.h>
#include <gpufmt/definitions.h>
#include <gpufmt/format.h>

#include <memory>
#include <vector>

namespace gpufmt {
    namespace internal {
        class BaseDecompressor;
    }

    class Decompressor {
    public:
        Decompressor() noexcept;
        Decompressor(gpufmt::Format compressedFormat) noexcept;
        Decompressor(const Decompressor &) noexcept = delete;
        Decompressor(Decompressor &&other) noexcept;
        ~Decompressor();

        Decompressor &operator=(const Decompressor &) noexcept = delete;
        Decompressor &operator=(Decompressor &&other) noexcept;

        [[nodiscard]]
        std::vector<gpufmt::byte> decompress(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, gpufmt::DecompressError &error) noexcept;

        gpufmt::DecompressError decompressTo(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, const gpufmt::Surface<gpufmt::byte> &decompressedSurface) noexcept;

    private:
        std::unique_ptr<internal::BaseDecompressor> mDecompressor;
        gpufmt::Format mCompressedFormat = gpufmt::Format::UNDEFINED;
    };
}