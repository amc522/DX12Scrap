// Classes:
//   FreeBlockTracker
//   ScopedReservedBlocks
//
// FreeBlockTracker:
//   Tracks what blocks are currently free in a fixed sized contiguous array.
//
// ScopedReservedBlocks:
//   RAII class that holds the range of a contiguous reservation in a FreeBlockTracker instance. Upon descrtuction,
//   ScopedReservedBlocks will release its reserved range back to FreeBlockTracker. ScopedReservedBlocks instances
//   cannot exist past the lifetime of the FreeBlockerTracker that created them.

#pragma once

#include <optional>
#include <utility>
#include <vector>

#include <tl/expected.hpp>

namespace scrap
{
class ScopedReservedBlocks;

class FreeBlockTracker
{
public:
    struct Range
    {
        size_t start = 0;
        size_t count = 0;

        bool operator==(Range other) const { return start == other.start && count == other.count; }

        [[nodiscard]] size_t inclusiveEnd() const { return start + count - 1; }
        [[nodiscard]] size_t exclusiveEnd() const { return start + count; }

        [[nodiscard]] bool contains(Range range) const
        {
            return start <= range.start && range.exclusiveEnd() <= exclusiveEnd();
        }

        [[nodiscard]] static std::optional<Range> merged(Range r1, Range r2)
        {
            if(r2.start > r1.exclusiveEnd() || r1.start > r2.exclusiveEnd()) { return std::nullopt; }

            size_t start = std::min(r1.start, r2.start);
            size_t exclusiveEnd = std::max(r1.exclusiveEnd(), r2.exclusiveEnd());

            return Range{start, exclusiveEnd - start};
        }

        bool merge(Range other)
        {
            auto result = merged(*this, other);
            if(result)
            {
                *this = result.value();
                return true;
            }

            return false;
        }
    };

    enum class Error
    {
        None,
        InsufficientCapacity
    };

    FreeBlockTracker() = default;
    explicit FreeBlockTracker(size_t capacity);
    FreeBlockTracker(const FreeBlockTracker&) = delete;
    FreeBlockTracker(FreeBlockTracker&&) noexcept = default;
    ~FreeBlockTracker() = default;

    FreeBlockTracker& operator=(const FreeBlockTracker&) = delete;
    FreeBlockTracker& operator=(FreeBlockTracker&&) noexcept = default;

    size_t getCapacity() const { return mCapacity; }
    size_t getReservedBlockCount() const { return mCapacity - mFreeCount; }
    size_t getFreeBlockCount() const { return mFreeCount; }

    tl::expected<ScopedReservedBlocks, Error> reserve(size_t contiguousBlockCount = 1);
    tl::expected<size_t, Error> unsafeReserve(size_t contiguousBlockCount = 1);
    void unsafeRelease(Range blockRange);
    void unsafeRelease(size_t startBlock, size_t blockCount);

private:
    std::vector<Range> mFreeRanges;
    size_t mCapacity = 0u;
    size_t mFreeCount = 0u;
};

class ScopedReservedBlocks
{
public:
    ScopedReservedBlocks() = default;
    ScopedReservedBlocks(FreeBlockTracker* tracker, FreeBlockTracker::Range range): mTracker(tracker), mRange(range) {}
    ScopedReservedBlocks(const ScopedReservedBlocks&) = delete;
    ScopedReservedBlocks(ScopedReservedBlocks&& other) noexcept
    {
        mTracker = other.mTracker;
        mRange = other.mRange;
        other.mTracker = nullptr;
    }

    ~ScopedReservedBlocks()
    {
        if(mTracker != nullptr) { mTracker->unsafeRelease(mRange); }
    }

    ScopedReservedBlocks& operator=(const ScopedReservedBlocks&) = delete;

    ScopedReservedBlocks& operator=(ScopedReservedBlocks&& other) noexcept
    {
        if(mTracker != nullptr) { mTracker->unsafeRelease(mRange); }
        mTracker = other.mTracker;
        mRange = other.mRange;
        other.mTracker = nullptr;

        return *this;
    }

    const FreeBlockTracker::Range& getRange() const { return mRange; }

private:
    FreeBlockTracker* mTracker = nullptr;
    FreeBlockTracker::Range mRange{0u, 0u};
};

} // namespace scrap