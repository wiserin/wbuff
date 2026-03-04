// NOLINTBEGIN
#include <gtest/gtest.h>
#include <memory_resource>

#include "wbuffer/buffer.hpp"

using namespace wbuffer;

// ──────────────────────────────────────────────────────────────
// Helpers
// ──────────────────────────────────────────────────────────────

static constexpr size_t kPoolSize = 64 * 1024;

struct Alloc {
    std::array<std::byte, kPoolSize> buf{};
    std::pmr::monotonic_buffer_resource res{buf.data(), buf.size(),
                                            std::pmr::null_memory_resource()};
    std::pmr::memory_resource* get() { return &res; }
};

// ──────────────────────────────────────────────────────────────
// Constructors
// ──────────────────────────────────────────────────────────────

TEST(WBufferConstruct, DefaultCtorIsEmpty) {
    WBuffer b;
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
    EXPECT_EQ(b.Capacity(), 0u);
}

TEST(WBufferConstruct, AllocOnlyCtorIsEmpty) {
    Alloc a;
    WBuffer b(a.get());
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferConstruct, CapacityCtorSetsCapacity) {
    Alloc a;
    WBuffer b(256, a.get());
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
    EXPECT_GE(b.Capacity(), 256u);
}

TEST(WBufferConstruct, CapacityLessThanMinUsesMin) {
    Alloc a;
    WBuffer b(1, a.get());
    EXPECT_EQ(b.Capacity(), 1);
}

// ──────────────────────────────────────────────────────────────
// SetAlloc
// ──────────────────────────────────────────────────────────────

TEST(WBufferSetAllocator, DefaultThenSetAllocatorAllowsPush) {
    Alloc a;
    WBuffer b;
    b.SetAlloc(a.get());
    b.PushBack(42);
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 42);
}

TEST(WBufferSetAllocator, SetAllocatorOnEmptyBufferDoesNotLeak) {
    Alloc a;
    WBuffer b;
    b.SetAlloc(a.get());
    b.SetAlloc(a.get());
    EXPECT_TRUE(b.Empty());
}

// ──────────────────────────────────────────────────────────────
// PushBack / PopBack / Front / Back
// ──────────────────────────────────────────────────────────────

TEST(WBufferPushPop, PushSingleElement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(7);
    EXPECT_FALSE(b.Empty());
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 7);
    EXPECT_EQ(b.Back(), 7);
}

TEST(WBufferPushPop, PushMultiplePreservesOrder) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    EXPECT_EQ(b.Size(), 3u);
    EXPECT_EQ(b.Front(), 1);
    EXPECT_EQ(b.Back(), 3);
}

TEST(WBufferPushPop, PopBackDecreasesSize) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(10); b.PushBack(20); b.PushBack(30);
    b.PopBack();
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Back(), 20);
}

TEST(WBufferPushPop, PopBackToEmpty) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(5);
    b.PopBack();
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferPushPop, MutateViaFrontRef) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(10);
    b.Front() = 99;
    EXPECT_EQ(b.Front(), 99);
}

TEST(WBufferPushPop, MutateViaBackRef) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    b.Back() = 77;
    EXPECT_EQ(b.Back(), 77);
    EXPECT_EQ(b[1], 77);
}

TEST(WBufferPushPop, PushTriggersGrowth) {
    Alloc a;
    WBuffer b(a.get());
    for (int i = 0; i < 300; ++i) b.PushBack(static_cast<uint8_t>(i & 0xFF));
    EXPECT_EQ(b.Size(), 300u);
    EXPECT_GE(b.Capacity(), 300u);
}

// ──────────────────────────────────────────────────────────────
// operator[]
// ──────────────────────────────────────────────────────────────

TEST(WBufferIndex, ReadWrite) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 16; ++i) b.PushBack(i);
    for (size_t i = 0; i < b.Size(); ++i)
        EXPECT_EQ(b[i], static_cast<uint8_t>(i));
}

TEST(WBufferIndex, MutateViaIndex) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    b[1] = 42;
    EXPECT_EQ(b[1], 42);
}

TEST(WBufferIndex, ConstOperatorIndex) {
    Alloc a;
    WBuffer tmp(a.get());
    tmp.PushBack(5); tmp.PushBack(10); tmp.PushBack(15);
    const WBuffer& cb = tmp;
    EXPECT_EQ(cb[0], 5);
    EXPECT_EQ(cb[1], 10);
    EXPECT_EQ(cb[2], 15);
}

// ──────────────────────────────────────────────────────────────
// Equality / Inequality
// ──────────────────────────────────────────────────────────────

TEST(WBufferEquality, TwoEmptyBuffersEqual) {
    WBuffer a, b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(WBufferEquality, EqualContents) {
    Alloc a1, a2;
    WBuffer x(a1.get()), y(a2.get());
    for (uint8_t i = 0; i < 5; ++i) { x.PushBack(i); y.PushBack(i); }
    EXPECT_EQ(x, y);
    EXPECT_FALSE(x != y);
}

TEST(WBufferEquality, DifferentSize) {
    Alloc a1, a2;
    WBuffer x(a1.get()), y(a2.get());
    x.PushBack(1);
    y.PushBack(1); y.PushBack(2);
    EXPECT_NE(x, y);
}

TEST(WBufferEquality, SameSizeDifferentContent) {
    Alloc a1, a2;
    WBuffer x(a1.get()), y(a2.get());
    x.PushBack(1);
    y.PushBack(2);
    EXPECT_NE(x, y);
}

// ──────────────────────────────────────────────────────────────
// Copy semantics
// ──────────────────────────────────────────────────────────────

TEST(WBufferCopy, CopyCtorDeepCopy) {
    Alloc a;
    WBuffer src(a.get());
    src.PushBack(1); src.PushBack(2); src.PushBack(3);

    WBuffer dst(src);
    EXPECT_EQ(src, dst);

    src.PushBack(4);
    EXPECT_NE(src, dst);
    EXPECT_EQ(dst.Size(), 3u);
}

TEST(WBufferCopy, CopyAssignDeepCopy) {
    Alloc a1, a2;
    WBuffer src(a1.get());
    for (uint8_t i = 0; i < 5; ++i) src.PushBack(i);

    WBuffer dst(a2.get());
    dst = src;
    EXPECT_EQ(src, dst);

    dst.PopBack();
    EXPECT_NE(src, dst);
}

TEST(WBufferCopy, SelfAssignmentSurvives) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(7);
    b = b;  // NOLINT
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 7);
}

// ──────────────────────────────────────────────────────────────
// Move semantics
// ──────────────────────────────────────────────────────────────

TEST(WBufferMove, MoveCtorTransfersData) {
    Alloc a;
    WBuffer src(a.get());
    src.PushBack(10); src.PushBack(20);

    WBuffer dst(std::move(src));
    EXPECT_EQ(dst.Size(), 2u);
    EXPECT_EQ(dst.Front(), 10);
    EXPECT_EQ(dst.Back(), 20);
    EXPECT_TRUE(src.Empty());
}

TEST(WBufferMove, MoveAssignTransfersData) {
    Alloc a;
    WBuffer src(a.get());
    for (uint8_t i = 0; i < 8; ++i) src.PushBack(i * 2);

    WBuffer dst;
    dst = std::move(src);
    EXPECT_EQ(dst.Size(), 8u);
    EXPECT_EQ(dst[0], 0);
    EXPECT_EQ(dst[7], 14);
    EXPECT_TRUE(src.Empty());
}

// ──────────────────────────────────────────────────────────────
// Swap
// ──────────────────────────────────────────────────────────────

TEST(WBufferSwap, MemberSwap) {
    Alloc a1, a2;
    WBuffer x(a1.get()), y(a2.get());
    x.PushBack(1); x.PushBack(2);
    y.PushBack(9);

    x.Swap(y);
    EXPECT_EQ(x.Size(), 1u);  EXPECT_EQ(x.Front(), 9);
    EXPECT_EQ(y.Size(), 2u);  EXPECT_EQ(y.Front(), 1);
}

TEST(WBufferSwap, StaticSwap) {
    Alloc a1, a2;
    WBuffer x(a1.get()), y(a2.get());
    x.PushBack(42);
    y.PushBack(1); y.PushBack(2); y.PushBack(3);

    WBuffer::Swap(x, y);
    EXPECT_EQ(x.Size(), 3u);
    EXPECT_EQ(y.Size(), 1u);
    EXPECT_EQ(y.Front(), 42);
}

TEST(WBufferSwap, SwapWithEmpty) {
    Alloc a;
    WBuffer x(a.get()), y;
    x.PushBack(5);

    x.Swap(y);
    EXPECT_TRUE(x.Empty());
    EXPECT_EQ(y.Size(), 1u);
    EXPECT_EQ(y.Front(), 5);
}

// ────────-─────────────────────────────────────────────────────
// Clear
// ──────────────────────────────────────────────────────────────

TEST(WBufferClear, ClearEmptiesBuffer) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 8; ++i) b.PushBack(i);
    b.Clear();
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferClear, ClearDoesNotFreeCapacity) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 8; ++i) b.PushBack(i);
    EXPECT_EQ(b.Capacity(), kDefaultCapacity);
    EXPECT_EQ(b.Size(), 8);
    b.Clear();
    EXPECT_EQ(b.Capacity(), 0);
}

TEST(WBufferClear, PushAfterClearWorks) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1);
    b.Clear();
    b.PushBack(2);
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 2);
}

// ──────────────────────────────────────────────────────────────
// Resize
// ──────────────────────────────────────────────────────────────

TEST(WBufferResize, GrowAddsZeroBytes) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1);
    b.Resize(5);
    EXPECT_EQ(b.Size(), 1);
    EXPECT_EQ(b.Capacity(), 5);
    EXPECT_EQ(b[0], 1);
}

TEST(WBufferResize, ShrinkPreservesData) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 10; ++i) b.PushBack(i);
    b.Resize(3);
    EXPECT_EQ(b.Size(), 3u);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
}

TEST(WBufferResize, ResizeToZeroMakesEmpty) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    b.Resize(0);
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferResize, ResizeSameSizeIsNoOp) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(99);
    b.Resize(1);
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 99);
}

// ──────────────────────────────────────────────────────────────
// Iterator
// ────────────────────────────────────────────────────────��─────

TEST(WBufferIterator, BeginEqualsEndOnEmpty) {
    Alloc a;
    WBuffer b(a.get());
    EXPECT_EQ(b.Begin(), b.End());
}

TEST(WBufferIterator, IterateAllElements) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 10; ++i) b.PushBack(i);
    uint8_t expected = 0;
    for (auto it = b.Begin(); it != b.End(); ++it)
        EXPECT_EQ(*it, expected++);
}

TEST(WBufferIterator, PrefixIncrement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    auto it = b.Begin();
    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(WBufferIterator, PostfixIncrement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    auto it = b.Begin();
    auto old = it++;
    EXPECT_EQ(*old, 1);
    EXPECT_EQ(*it, 2);
}

TEST(WBufferIterator, PrefixDecrement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    auto it = b.End();
    --it;
    EXPECT_EQ(*it, 2);
}

TEST(WBufferIterator, PostfixDecrement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    auto it = b.Begin();
    ++it;
    auto old = it--;
    EXPECT_EQ(*old, 2);
    EXPECT_EQ(*it, 1);
}

TEST(WBufferIterator, PlusEqAndMinusEq) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i);
    auto it = b.Begin();
    it += 3;
    EXPECT_EQ(*it, 3);
    it -= 2;
    EXPECT_EQ(*it, 1);
}

TEST(WBufferIterator, PlusAndMinus) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i * 3);
    auto it = b.Begin();
    EXPECT_EQ(*(it + 4), 12);
    EXPECT_EQ(*(it + 4 - 2), 6);
}

TEST(WBufferIterator, Distance) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i);
    EXPECT_EQ(b.End() - b.Begin(), 5u);
}

TEST(WBufferIterator, SubscriptOperator) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i * 10);
    auto it = b.Begin();
    EXPECT_EQ(it[0], 0);
    EXPECT_EQ(it[3], 30);
}

// ──────────────────────────────────────────────────────────────
// Iterator
// ──────────────────────────────────────────────────────────────

TEST(WBufferIterator, EqualityOnSamePosition) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1);
    auto it1 = b.Begin();
    auto it2 = b.Begin();
    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 != it2);
    EXPECT_TRUE(it1 <= it2);
    EXPECT_TRUE(it1 >= it2);
    EXPECT_FALSE(it1 < it2);
    EXPECT_FALSE(it1 > it2);
}

TEST(WBufferIterator, OrderComparisons) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    auto it1 = b.Begin();
    auto it2 = it1; ++it2;
    EXPECT_TRUE(it1 < it2);
    EXPECT_TRUE(it2 > it1);
    EXPECT_TRUE(it1 <= it2);
    EXPECT_TRUE(it2 >= it1);
    EXPECT_TRUE(it1 != it2);
}

// ──────────────────────────────────────────────────────────────
// Insert
// ──────────────────────────────────────────────────────────────

TEST(WBufferInsert, InsertAtBegin) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    auto it = b.Insert(b.Begin(), 0);
    EXPECT_EQ(*it, 0);
    EXPECT_EQ(b.Front(), 0);
    EXPECT_EQ(b.Size(), 3u);
}

TEST(WBufferInsert, InsertAtEnd) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1);
    auto it = b.Insert(b.End(), 2);
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(b.Back(), 2);
    EXPECT_EQ(b.Size(), 2u);
}

TEST(WBufferInsert, InsertInMiddle) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(0); b.PushBack(1); b.PushBack(2);
    auto mid = b.Begin(); ++mid;  // points to 1
    auto it = b.Insert(mid, 99);
    EXPECT_EQ(*it, 99);
    EXPECT_EQ(b.Size(), 4u);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 99);
    EXPECT_EQ(b[2], 1);
    EXPECT_EQ(b[3], 2);
}

TEST(WBufferInsert, InsertCountZero) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1);
    b.Insert(b.Begin(), 0u, 99);
    EXPECT_EQ(b.Size(), 1u);
}

TEST(WBufferInsert, InsertCountMultiple) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1);
    b.Insert(b.End(), 3u, 7);
    EXPECT_EQ(b.Size(), 4u);
    EXPECT_EQ(b[1], 7);
    EXPECT_EQ(b[2], 7);
    EXPECT_EQ(b[3], 7);
}

TEST(WBufferInsert, InsertRangeFromOtherBuffer) {
    Alloc a1, a2;
    WBuffer src(a1.get());
    src.PushBack(10); src.PushBack(20); src.PushBack(30);

    WBuffer dst(a2.get());
    dst.PushBack(1);
    dst.Insert(dst.End(), src.Begin(), src.End());
    EXPECT_EQ(dst.Size(), 4u);
    EXPECT_EQ(dst[1], 10);
    EXPECT_EQ(dst[3], 30);
}

TEST(WBufferInsert, InsertEmptyRange) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(5);
    b.Insert(b.Begin(), b.End(), b.End());
    EXPECT_EQ(b.Size(), 1u);
}

// ──────────────────────────────────────────────────────────────
// Erase
// ──────────────────────────────────────────────────────────────

TEST(WBufferErase, EraseFirstElement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    auto next = b.Erase(b.Begin());
    EXPECT_EQ(*next, 2);
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Front(), 2);
}

TEST(WBufferErase, EraseLastElement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    auto it = b.End(); --it;
    auto next = b.Erase(it);
    EXPECT_EQ(next, b.End());
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Back(), 2);
}

TEST(WBufferErase, EraseMiddleElement) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    auto it = b.Begin(); ++it;
    auto next = b.Erase(it);
    EXPECT_EQ(*next, 3);
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 3);
}

TEST(WBufferErase, EraseAllViaRange) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i);
    auto after = b.Erase(b.Begin(), b.End());
    EXPECT_EQ(after, b.End());
    EXPECT_TRUE(b.Empty());
}

TEST(WBufferErase, EraseEmptyRange) {
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1); b.PushBack(2);
    auto after = b.Erase(b.Begin(), b.Begin());
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(*after, 1);
}

TEST(WBufferErase, EraseRangeInMiddle) {
    Alloc a;
    WBuffer b(a.get());
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i);
    auto beg = b.Begin(); ++beg;
    auto end = beg; ++end; ++end;
    b.Erase(beg, end);
    EXPECT_EQ(b.Size(), 3u);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 3);
    EXPECT_EQ(b[2], 4);
}

// ──────────────────────────────────────────────────────────────
// Static setters
// ──────────────────────────────────────────────────────────────

TEST(WBufferStaticSetters, SetDefaultCapacityAffectsNewAlloc) {
    WBuffer::SetDefaultCapacity(256);
    Alloc a;
    WBuffer b(a.get());
    b.PushBack(1);
    EXPECT_GE(b.Capacity(), 256u);
    WBuffer::SetDefaultCapacity(kDefaultCapacity);
}

TEST(WBufferStaticSetters, SetResizeScaleAffectsGrowth) {
    WBuffer::SetResizeScale(4);
    Alloc a;
    WBuffer b(a.get());
    for (int i = 0; i <= static_cast<int>(kDefaultCapacity); ++i)
        b.PushBack(static_cast<uint8_t>(i & 0xFF));
    EXPECT_GE(b.Capacity(), kDefaultCapacity * 4);
    WBuffer::SetResizeScale(kResizeScale);
}

// ──────────────────────────────────────────────────────────────
// Stress
// ──────────────────────────────────────────────────────────────

TEST(WBufferStress, PushBackManyElements) {
    std::pmr::unsynchronized_pool_resource pool;
    WBuffer b(&pool);
    constexpr size_t N = 10'000;
    for (size_t i = 0; i < N; ++i) b.PushBack(static_cast<uint8_t>(i & 0xFF));
    EXPECT_EQ(b.Size(), N);
    for (size_t i = 0; i < N; ++i)
        EXPECT_EQ(b[i], static_cast<uint8_t>(i & 0xFF));
}

TEST(WBufferStress, AlternatePushPopStability) {
    std::pmr::unsynchronized_pool_resource pool;
    WBuffer b(&pool);
    for (int round = 0; round < 100; ++round) {
        for (uint8_t i = 0; i < 50; ++i) b.PushBack(i);
        for (int i = 0; i < 50; ++i) b.PopBack();
    }
    EXPECT_TRUE(b.Empty());
}

// NOLINTEND