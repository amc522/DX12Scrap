#include <gpufmt/write.h>
#include <gpufmt/storage.h>
#include <gpufmt/traits.h>
#include <gpufmt/visit.h>

#include <vector>

namespace gpufmt {
    namespace internal {
        class BaseWriter {
        public:
            virtual ~BaseWriter() = default;

            [[nodiscard]]
            virtual std::vector<gpufmt::byte> write(const gpufmt::SampleVariant &variant, WriteError &error) noexcept = 0;

            [[nodiscard]]
            virtual WriteError writeTo(const gpufmt::SampleVariant &variant, gpufmt::span<gpufmt::byte> dest) noexcept = 0;
        };

        template<gpufmt::Format FormatV>
        class WriterT : public BaseWriter {
        public:
            using Storage = gpufmt::FormatStorage<FormatV>;
            using Traits = gpufmt::FormatTraits<FormatV>;

            ~WriterT() final = default;

            [[nodiscard]]
            std::vector<gpufmt::byte> write([[maybe_unused]] const gpufmt::SampleVariant &variant, WriteError &error) noexcept override {
                if constexpr(!Storage::Writeable) {
                    error = gpufmt::WriteError::FormatNotWriteable;
                    return {};
                } else {
                    std::vector<gpufmt::byte> dest;
                    dest.resize(Traits::BlockByteSize);

                    error = writeTo(variant, gpufmt::span<gpufmt::byte>(dest));
                    return dest;
                }
            }

            [[nodiscard]]
            WriteError writeTo([[maybe_unused]] const gpufmt::SampleVariant &variant, [[maybe_unused]] gpufmt::span<gpufmt::byte> dest) noexcept override {
                if constexpr(!Storage::Writeable) {
                    return gpufmt::WriteError::FormatNotWriteable;
                } else {
                    if(Traits::BlockByteSize > dest.size_bytes()) {
                        return gpufmt::WriteError::DestinationTooSmall;
                    }

                    std::visit([dest](const auto &sample) {
                        auto destSample = static_cast<Traits::WideSampleType>(sample);

                        typename Traits::BlockType &destBlock = reinterpret_cast<Traits::BlockType &>(*dest.data());
                        destBlock = Storage::storeBlock(gpufmt::span<Traits::WideSampleType, 1>(&destSample, 1));
                    }, variant);

                    return gpufmt::WriteError::None;
                }
            }
        };

        template<gpufmt::Format FormatV>
        class MakeWriter {
        public:
            std::unique_ptr<BaseWriter> operator()() const noexcept {
                return std::make_unique<WriterT<FormatV>>();
            }
        };
    }

    Writer::Writer() noexcept {}

    Writer::Writer(Writer &&other) noexcept
        : mWriter(std::move(other.mWriter))
        , mFormat(other.mFormat)
    {
    }

    Writer::Writer(gpufmt::Format format) noexcept
        : mFormat(format)
    {
        mWriter = gpufmt::visitFormat<gpufmt::internal::MakeWriter>(format);
    }

    Writer::~Writer() {}

    Writer& Writer::operator=(Writer &&other) noexcept {
        mFormat = other.mFormat;
        other.mFormat = gpufmt::Format::UNDEFINED;
        
        mWriter = std::move(other.mWriter);
        return *this;
    }

    [[nodiscard]]
    gpufmt::Format Writer::format() const noexcept {
        return mFormat;
    }

    [[nodiscard]]
    std::vector<gpufmt::byte> Writer::write(const gpufmt::SampleVariant &variant, WriteError &error) const noexcept {
        return mWriter->write(variant, error);
    }

    [[nodiscard]]
    WriteError Writer::writeTo(const gpufmt::SampleVariant &variant, gpufmt::span<gpufmt::byte> dest) const noexcept {
        return mWriter->writeTo(variant, dest);
    }
}