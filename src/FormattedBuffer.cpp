#include "FormattedBuffer.h"

#include <gpufmt/traits.h>

namespace scrap
{
size_t FormattedBufferSpan::elementCount() const
{
    return buffer.size_bytes() / gpufmt::formatInfo(format).blockByteSize;
}

nonstd::span<std::byte> FormattedBufferSpan::accessElement(size_t index)
{
    const size_t elementByteSize = gpufmt::formatInfo(format).blockByteSize;
    return buffer.subspan(elementByteSize * index, elementByteSize);
}

nonstd::span<const std::byte> FormattedBufferSpan::getElement(size_t index) const
{
    const size_t elementByteSize = gpufmt::formatInfo(format).blockByteSize;
    return buffer.subspan(elementByteSize * index, elementByteSize);
}

FormattedBufferSpan FormattedBufferSpan::formattedSubspan(size_t elementOffset, size_t elementCount) const
{
    const size_t elementByteSize = gpufmt::formatInfo(format).blockByteSize;
    return {format, buffer.subspan(elementOffset * elementByteSize, (elementCount != nonstd::dynamic_extent) ? elementCount * elementByteSize : nonstd::dynamic_extent)};
}

size_t FormattedBufferView::elementCount() const
{
    return buffer.size_bytes() / gpufmt::formatInfo(format).blockByteSize;
}

nonstd::span<const std::byte> FormattedBufferView::getElement(size_t index) const
{
    const size_t elementByteSize = gpufmt::formatInfo(format).blockByteSize;
    return buffer.subspan(elementByteSize * index, elementByteSize);
}

FormattedBufferView FormattedBufferView::formattedSubview(size_t elementOffset, size_t elementCount) const
{
    const size_t elementByteSize = gpufmt::formatInfo(format).blockByteSize;
    return {format, buffer.subspan(elementOffset * elementByteSize, (elementCount != nonstd::dynamic_extent) ? elementCount * elementByteSize : nonstd::dynamic_extent)};
}
} // namespace scrap