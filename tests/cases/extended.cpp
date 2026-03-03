// NOLINTBEGIN
#include <gtest/gtest.h>
#include <memory_resource>

#include "wbuffer/buffer.hpp"

using namespace wbuffer;

// ──────────────────────────────────────────────
// Constructors
// ──────────────────────────────────────────────

TEST(WBufferConstruct, DefaultIsEmpty) {
    WBuffer b;
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferConstruct, WithCapacityIsEmpty) {
    std::pmr::memory_resource* default_resource = std::pmr::get_default_resource();
    WBuffer b(64, default_resource);
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferConstruct, WithPmrResourceDoesNotCrash) {
    std::pmr::monotonic_buffer_resource pool(1024);
    WBuffer b(64, &pool);
    b.PushBack(42);
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 42);
}

// ──────────────────────────────────────────────
// Copy semantics
// ──────────────────────────────────────────────

TEST(WBufferCopy, CopyConstructorDeepCopy) {
    WBuffer a;
    a.PushBack(1);
    a.PushBack(2);
    a.PushBack(3);

    WBuffer b(a);
    EXPECT_EQ(a, b);

    // Независимость: мутируем a — b не меняется
    a.PushBack(4);
    EXPECT_NE(a, b);
    EXPECT_EQ(b.Size(), 3u);
}

TEST(WBufferCopy, CopyAssignmentDeepCopy) {
    WBuffer a;
    for (uint8_t i = 0; i < 5; ++i) a.PushBack(i);

    WBuffer b;
    b = a;
    EXPECT_EQ(a, b);

    b.PopBack();
    EXPECT_NE(a, b);
}

TEST(WBufferCopy, SelfAssignment) {
    WBuffer a;
    a.PushBack(7);
    // UB если не обработан self-assign — должен выжить
    a = a;  // NOLINT
    EXPECT_EQ(a.Size(), 1u);
    EXPECT_EQ(a.Front(), 7);
}

// ──────────────────────────────────────────────
// Move semantics
// ──────────────────────────────────────────────

TEST(WBufferMove, MoveConstructorTransfersData) {
    WBuffer a;
    a.PushBack(10);
    a.PushBack(20);

    WBuffer b(std::move(a));
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Front(), 10);
    EXPECT_EQ(b.Back(), 20);
    // a должен быть в валидном пустом состоянии
    EXPECT_TRUE(a.Empty());
}

TEST(WBufferMove, MoveAssignmentTransfersData) {
    WBuffer a;
    for (uint8_t i = 0; i < 8; ++i) a.PushBack(i * 2);

    WBuffer b;
    b = std::move(a);
    EXPECT_EQ(b.Size(), 8u);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[7], 14);
    EXPECT_TRUE(a.Empty());
}

// ──────────────────────────────────────────────
// Equality / Inequality
// ──────────────────────────────────────────────

TEST(WBufferEquality, EqualBuffers) {
    WBuffer a, b;
    for (uint8_t i = 0; i < 5; ++i) {
        a.PushBack(i);
        b.PushBack(i);
    }
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(WBufferEquality, UnequalBuffersDifferentSize) {
    WBuffer a, b;
    a.PushBack(1);
    b.PushBack(1);
    b.PushBack(2);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(WBufferEquality, UnequalBuffersSameSizeDifferentContent) {
    WBuffer a, b;
    a.PushBack(1);
    b.PushBack(2);
    EXPECT_FALSE(a == b);
}

TEST(WBufferEquality, TwoEmptyBuffersAreEqual) {
    WBuffer a, b;
    EXPECT_EQ(a, b);
}

// ──────────────────────────────────────────────
// Swap
// ──────────────────────────────────────────────

TEST(WBufferSwap, MemberSwap) {
    WBuffer a, b;
    a.PushBack(1); a.PushBack(2);
    b.PushBack(9);

    a.Swap(b);
    EXPECT_EQ(a.Size(), 1u);
    EXPECT_EQ(a.Front(), 9);
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Front(), 1);
}

TEST(WBufferSwap, StaticSwap) {
    WBuffer a, b;
    a.PushBack(42);
    b.PushBack(1); b.PushBack(2); b.PushBack(3);

    WBuffer::Swap(a, b);
    EXPECT_EQ(a.Size(), 3u);
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 42);
}

TEST(WBufferSwap, SwapWithEmpty) {
    WBuffer a, b;
    a.PushBack(5);

    a.Swap(b);
    EXPECT_TRUE(a.Empty());
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 5);
}

// ──────────────────────────────────────────────
// Resize
// ──────────────────────────────────────────────

TEST(WBufferResize, GrowAddsZeroBytes) {
    WBuffer b;
    b.PushBack(1);
    b.Resize(5);
    EXPECT_EQ(b.Size(), 5u);
    // Новые байты должны быть 0 (типичное поведение)
    for (size_t i = 1; i < 5; ++i) {
        EXPECT_EQ(b[i], 0);
    }
}

TEST(WBufferResize, ShrinkPreservesData) {
    WBuffer b;
    for (uint8_t i = 0; i < 10; ++i) b.PushBack(i);
    b.Resize(3);
    EXPECT_EQ(b.Size(), 3u);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
}

TEST(WBufferResize, ResizeToZeroMakesEmpty) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2);
    b.Resize(0);
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferResize, ResizeSameSizeIsNoOp) {
    WBuffer b;
    b.PushBack(99);
    b.Resize(1);
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 99);
}

// ──────────────────────────────────────────────
// Clear
// ──────────────────────────────────────────────

TEST(WBufferClear, ClearEmptiesBuffer) {
    WBuffer b;
    for (uint8_t i = 0; i < 8; ++i) b.PushBack(i);
    b.Clear();
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferClear, ClearThenPushWorks) {
    WBuffer b;
    b.PushBack(1);
    b.Clear();
    b.PushBack(2);
    EXPECT_EQ(b.Size(), 1u);
    EXPECT_EQ(b.Front(), 2);
}

// ──────────────────────────────────────────────
// Front / Back
// ──────────────────────────────────────────────

TEST(WBufferFrontBack, SingleElement) {
    WBuffer b;
    b.PushBack(77);
    EXPECT_EQ(b.Front(), 77);
    EXPECT_EQ(b.Back(), 77);
}

TEST(WBufferFrontBack, FrontAndBackAfterMultiplePush) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    EXPECT_EQ(b.Front(), 1);
    EXPECT_EQ(b.Back(), 3);
}

TEST(WBufferFrontBack, MutateViaFrontRef) {
    WBuffer b;
    b.PushBack(10);
    b.Front() = 55;
    EXPECT_EQ(b.Front(), 55);
}

TEST(WBufferFrontBack, MutateViaBackRef) {
    WBuffer b;
    b.PushBack(10); b.PushBack(20);
    b.Back() = 99;
    EXPECT_EQ(b.Back(), 99);
    EXPECT_EQ(b[1], 99);
}

// ──────────────────────────────────────────────
// operator[]
// ───────────────────────────��──────────────────

TEST(WBufferIndex, ConstOperatorIndex) {
    WBuffer tmp;
    tmp.PushBack(5); tmp.PushBack(10); tmp.PushBack(15);
    const WBuffer b = tmp;
    EXPECT_EQ(b[0], 5);
    EXPECT_EQ(b[1], 10);
    EXPECT_EQ(b[2], 15);
}

TEST(WBufferIndex, MutateViaOperatorIndex) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    b[1] = 42;
    EXPECT_EQ(b[1], 42);
}

// ──────────────────────────────────────────────
// Iterator
// ──────────────────────────────────────────────

TEST(WBufferIterator, PostfixIncrement) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2);
    auto it = b.Begin();
    auto old = it++;  // постфикс — вернул старое
    EXPECT_EQ(*old, 1);
    EXPECT_EQ(*it, 2);
}

TEST(WBufferIterator, PostfixDecrement) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2);
    auto it = b.Begin();
    ++it;
    auto old = it--;
    EXPECT_EQ(*old, 2);
    EXPECT_EQ(*it, 1);
}

TEST(WBufferIterator, ArithmeticPlusMinus) {
    WBuffer b;
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i * 3);

    auto it = b.Begin();
    auto it2 = it + 4;
    EXPECT_EQ(*it2, 12);

    auto it3 = it2 - 2;
    EXPECT_EQ(*it3, 6);
}

TEST(WBufferIterator, MinusEqOperator) {
    WBuffer b;
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i);
    auto it = b.End();
    it -= 1;
    EXPECT_EQ(*it, 4);
}

TEST(WBufferIterator, SubscriptOperator) {
    WBuffer b;
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i * 10);
    auto it = b.Begin();
    EXPECT_EQ(it[0], 0);
    EXPECT_EQ(it[3], 30);
}

TEST(WBufferIterator, BeginEqualsEndOnEmpty) {
    WBuffer b;
    EXPECT_EQ(b.Begin(), b.End());
}

TEST(WBufferIterator, IterateAllElements) {
    WBuffer b;
    for (uint8_t i = 0; i < 10; ++i) b.PushBack(i);

    uint8_t expected = 0;
    for (auto it = b.Begin(); it != b.End(); ++it) {
        EXPECT_EQ(*it, expected++);
    }
}

TEST(WBufferIterator, FullEqualitySet) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2);
    auto it1 = b.Begin();
    auto it2 = b.Begin();
    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 != it2);
    EXPECT_TRUE(it1 <= it2);
    EXPECT_TRUE(it1 >= it2);
    EXPECT_FALSE(it1 < it2);
    EXPECT_FALSE(it1 > it2);
}

// ──────────────────────────────────────────────
// Insert
// ──────────────────────────────────────────────

TEST(WBufferInsert, InsertAtBegin) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2);
    auto it = b.Insert(b.Begin(), 0);
    EXPECT_EQ(*it, 0);
    EXPECT_EQ(b.Front(), 0);
    EXPECT_EQ(b.Size(), 3u);
}

TEST(WBufferInsert, InsertAtEnd) {
    WBuffer b;
    b.PushBack(1);
    auto it = b.Insert(b.End(), 2);
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(b.Back(), 2);
    EXPECT_EQ(b.Size(), 2u);
}

TEST(WBufferInsert, InsertZeroCount) {
    WBuffer b;
    b.PushBack(1);
    b.Insert(b.Begin(), 0, 99);  // вставить 0 штук
    EXPECT_EQ(b.Size(), 1u);
}

TEST(WBufferInsert, InsertRangeFromOtherBuffer) {
    WBuffer src;
    src.PushBack(10); src.PushBack(20); src.PushBack(30);

    WBuffer dst;
    dst.PushBack(1);
    dst.Insert(dst.End(), src.Begin(), src.End());

    EXPECT_EQ(dst.Size(), 4u);
    EXPECT_EQ(dst[1], 10);
    EXPECT_EQ(dst[3], 30);
}

TEST(WBufferInsert, InsertRangeEmptyRange) {
    WBuffer b;
    b.PushBack(5);
    b.Insert(b.Begin(), b.End(), b.End());  // пустой диапазон
    EXPECT_EQ(b.Size(), 1u);
}

// ──────────────────────────────────────────────
// Erase
// ──────────────────────────────────────────────

TEST(WBufferErase, EraseFirstElement) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    auto next = b.Erase(b.Begin());
    EXPECT_EQ(*next, 2);
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Front(), 2);
}

TEST(WBufferErase, EraseLastElement) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2); b.PushBack(3);
    auto it = b.End(); --it;
    auto next = b.Erase(it);
    EXPECT_EQ(next, b.End());
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Back(), 2);
}

TEST(WBufferErase, EraseAllViaRange) {
    WBuffer b;
    for (uint8_t i = 0; i < 5; ++i) b.PushBack(i);
    auto after = b.Erase(b.Begin(), b.End());
    EXPECT_EQ(after, b.End());
    EXPECT_TRUE(b.Empty());
}

TEST(WBufferErase, EraseEmptyRange) {
    WBuffer b;
    b.PushBack(1); b.PushBack(2);
    auto after = b.Erase(b.Begin(), b.Begin());  // пустой диапазон
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(*after, 1);
}

// ──────────────────────────────────────────────
// Stress / growth
// ──────────────────────────────────────────────

TEST(WBufferStress, PushBackManyElements) {
    WBuffer b;
    constexpr size_t N = 10000;
    for (size_t i = 0; i < N; ++i) b.PushBack(static_cast<uint8_t>(i & 0xFF));
    EXPECT_EQ(b.Size(), N);
    for (size_t i = 0; i < N; ++i) {
        EXPECT_EQ(b[i], static_cast<uint8_t>(i & 0xFF));
    }
}

TEST(WBufferStress, AlternatePushPopStability) {
    WBuffer b;
    for (int round = 0; round < 100; ++round) {
        for (uint8_t i = 0; i < 50; ++i) b.PushBack(i);
        for (int i = 0; i < 50; ++i) b.PopBack();
    }
    EXPECT_TRUE(b.Empty());
}
// NOLINTEND