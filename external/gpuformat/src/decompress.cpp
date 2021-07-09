#include <gpufmt/decompress.h>

#include <gpufmt/traits.h>
#include <gpufmt/storage.h>
#include <gpufmt/visit.h>

namespace gpufmt {
    namespace internal {
        class BaseDecompressor {
        public:
            virtual ~BaseDecompressor() = default;

            [[nodiscard]]
            virtual std::vector<gpufmt::byte> decompress(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, gpufmt::DecompressError &error) noexcept = 0;

            virtual gpufmt::DecompressError decompressTo(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, const gpufmt::Surface<gpufmt::byte> &decompressedSurface) noexcept = 0;
        };

        template<gpufmt::Format FormatV>
        class DecompressorT : public BaseDecompressor {
        public:
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
            using DecompressedTraitsAlt = FormatTraits<Traits::info.decompressedFormatAlt>;

            using Storage = FormatStorage<FormatV>;

            ~DecompressorT() override = default;

            [[nodiscard]]
            std::vector<gpufmt::byte> decompress(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, gpufmt::DecompressError &error) noexcept override {
                if constexpr(!Storage::Decompressible) {
                    error = DecompressError::FormatNotDecompressible;
                    return {};
                } else {
                    std::vector<gpufmt::byte> decompressedData(compressedSurface.blockCount() * Traits::BlockTexelCount * DecompressedTraits::BlockByteSize);

                    Surface<gpufmt::byte> decompressedSurface;
                    decompressedSurface.blockData = decompressedData;
                    decompressedSurface.extentInBlocks = compressedSurface.extentInBlocks * Traits::BlockExtent;

                    error = decompressTo(compressedSurface, decompressedSurface);
                    return decompressedData;
                }
            }

            gpufmt::DecompressError decompressTo(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, const gpufmt::Surface<gpufmt::byte> &decompressedSurface) noexcept override {
                if constexpr(!Storage::Decompressible) {
                    return DecompressError::FormatNotDecompressible;
                } else {
                    const size_t decompressedByteSize = decompressedSurface.blockCount() * DecompressedTraits::BlockByteSize;

                    if(decompressedSurface.blockData.size_bytes() < decompressedByteSize) {
                        return DecompressError::DestinationTooSmall;
                    }

                    const Extent maxDecompressedExtent{ compressedSurface.extentInBlocks * Traits::BlockExtent };
                    const Extent minDecompressedExtent{ maxDecompressedExtent - (Traits::BlockExtent - Extent{1, 1, 1}) };

                    if(decompressedSurface.extentInBlocks.x < minDecompressedExtent.x ||
                       decompressedSurface.extentInBlocks.y < minDecompressedExtent.y ||
                       decompressedSurface.extentInBlocks.z < minDecompressedExtent.z)
                    {
                        return DecompressError::DestinationTooSmall;
                    }
                    
                    auto typedCompressedSurface = compressedSurface.getAs<const typename Traits::BlockType>();
                    auto typedDecompressedSurface = decompressedSurface.getAs<typename DecompressedTraits::BlockType>();
                    
                    return Storage::decompress(typedCompressedSurface, typedDecompressedSurface);
                }
            }

        };

        template<gpufmt::Format FormatV>
        class MakeDecompressor {
        public:
            std::unique_ptr<BaseDecompressor> operator()() const noexcept {
                return std::make_unique<DecompressorT<FormatV>>();
            }
        };
    }


    Decompressor::Decompressor() noexcept {}

    Decompressor::Decompressor(gpufmt::Format compressedFormat) noexcept
        : mCompressedFormat(compressedFormat)
    {
        mDecompressor = gpufmt::visitFormat<internal::MakeDecompressor>(compressedFormat);
    }

    Decompressor::Decompressor(Decompressor &&other) noexcept
        : mDecompressor(std::move(other.mDecompressor))
        , mCompressedFormat(other.mCompressedFormat)
    {}

    Decompressor::~Decompressor() {}

    Decompressor &Decompressor::operator=(Decompressor &&other) noexcept {
        mDecompressor = std::move(other.mDecompressor);
        mCompressedFormat = other.mCompressedFormat;
        other.mCompressedFormat = gpufmt::Format::UNDEFINED;

        return *this;
    }

    std::vector<gpufmt::byte> Decompressor::decompress(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, gpufmt::DecompressError &error) noexcept {
        return mDecompressor->decompress(compressedSurface, error);
    }

    gpufmt::DecompressError Decompressor::decompressTo(const gpufmt::Surface<const gpufmt::byte> &compressedSurface, const gpufmt::Surface<gpufmt::byte> &decompressedSurface) noexcept {
        return mDecompressor->decompressTo(compressedSurface, decompressedSurface);
    }

}