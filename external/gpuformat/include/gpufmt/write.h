#pragma once

#include <gpufmt/config.h>
#include <gpufmt/definitions.h>
#include <gpufmt/format.h>
#include <gpufmt/internal/sample_variant.h>

#include <memory>
#include <vector>

namespace gpufmt {
    namespace internal {
        class BaseWriter;
    }

    class Writer {
    public:
        Writer() noexcept;
        Writer(const Writer&) noexcept = delete;
        Writer(Writer &&other) noexcept;
        Writer(gpufmt::Format format) noexcept;
        ~Writer();

        Writer& operator=(const Writer&) noexcept = delete;
        Writer& operator=(Writer &&other) noexcept;

        [[nodiscard]]
        gpufmt::Format format() const noexcept;

        [[nodiscard]]
        std::vector<gpufmt::byte> write(const gpufmt::SampleVariant &variant, WriteError &error) const noexcept;

        [[nodiscard]]
        WriteError writeTo(const gpufmt::SampleVariant &variant, gpufmt::span<gpufmt::byte> dest) const noexcept;

    private:
        std::unique_ptr<gpufmt::internal::BaseWriter> mWriter;
        gpufmt::Format mFormat = gpufmt::Format::UNDEFINED;
    };
}