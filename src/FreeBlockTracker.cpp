#include "FreeBlockTracker.h"

namespace scrap
{
FreeBlockTracker::FreeBlockTracker(size_t capacity)
    : mFreeRanges(1, Range{size_t(0), size_t(capacity)}), mCapacity(capacity), mFreeCount(capacity)
{
}

tl::expected<ScopedReservedBlocks, FreeBlockTracker::Error> FreeBlockTracker::reserve(size_t contiguousBlockCount)
{
    auto result = unsafeReserve(contiguousBlockCount);
    if(!result) { return tl::make_unexpected(result.error()); }

    return ScopedReservedBlocks{this, Range{result.value(), contiguousBlockCount}};
}

tl::expected<size_t, FreeBlockTracker::Error> FreeBlockTracker::unsafeReserve(size_t contiguousBlockCount)
{
    auto findItr = std::find_if(mFreeRanges.begin(), mFreeRanges.end(), [contiguousBlockCount](const Range& freeRange) {
        return freeRange.count >= contiguousBlockCount;
    });

    if(findItr == mFreeRanges.end()) { return tl::make_unexpected(Error::InsufficientCapacity); }

    size_t startBlock = findItr->start;
    findItr->start += contiguousBlockCount;
    findItr->count -= contiguousBlockCount;

    mFreeCount -= contiguousBlockCount;

    if(findItr->count == 0) { mFreeRanges.erase(findItr); }

    return startBlock;
}

void FreeBlockTracker::unsafeRelease(Range blockRange)
{
    if(blockRange.count == 0) { return; }
    if(blockRange.inclusiveEnd() >= mCapacity) { return; }

    const size_t releasedEndBlock = blockRange.inclusiveEnd();

    for(auto itr = mFreeRanges.begin(); itr != mFreeRanges.end(); ++itr)
    {
        if(itr->start == releasedEndBlock + 1)
        {
            itr->start = blockRange.start;
            itr->count += blockRange.count;
            mFreeCount += blockRange.count;
            return;
        }
        else if(itr->inclusiveEnd() + 1 == blockRange.start)
        {
            itr->count += blockRange.count;
            mFreeCount += blockRange.count;
            return;
        }
        else if(itr->start > blockRange.start)
        {
            mFreeRanges.insert(itr, blockRange);
            mFreeCount += blockRange.count;
            return;
        }
    }

    mFreeRanges.push_back(blockRange);
    mFreeCount += blockRange.count;
}

void FreeBlockTracker::unsafeRelease(size_t startBlock, size_t blockCount)
{
    unsafeRelease(Range{startBlock, blockCount});
}

} // namespace scrap