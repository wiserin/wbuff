// NOLINTBEGIN
#include <gtest/gtest.h>

#include "wbuffer/buffer.hpp"

using namespace wbuffer;

TEST(WBufferBasic, DefaultConstructedEmpty) {
    WBuffer b;
    EXPECT_TRUE(b.Empty());
    EXPECT_EQ(b.Size(), 0u);
}

TEST(WBufferBasic, PushPopFrontBack) {
    WBuffer b;
    b.PushBack(10);
    b.PushBack(20);
    b.PushBack(30);

    EXPECT_FALSE(b.Empty());
    EXPECT_EQ(b.Size(), 3u);
    EXPECT_EQ(b.Front(), 10);
    EXPECT_EQ(b.Back(), 30);

    b.PopBack();
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(b.Back(), 20);
}

TEST(WBufferIndexing, OperatorIndex) {
    WBuffer b;
    for (uint8_t i = 0; i < 16; ++i) b.PushBack(i);
    for (size_t i = 0; i < b.Size(); ++i) {
        EXPECT_EQ(b[i], static_cast<uint8_t>(i));
    }
}

TEST(WBufferIterator, IncrementDecrementAndDistance) {
    WBuffer b;
    for (uint8_t i = 1; i <= 5; ++i) b.PushBack(i);

    auto begin = b.Begin();
    auto end = b.End();

    EXPECT_EQ(end - begin, 5u);

    auto it = begin;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    it += 2;
    EXPECT_EQ(*it, 4);
    --it;
    EXPECT_EQ(*it, 3);
}

TEST(WBufferInsertErase, InsertSingleAndRangeErase) {
    WBuffer b;
    for (uint8_t i = 0; i < 4; ++i) b.PushBack(i);

    auto it = b.Begin(); // pointing to 0
    ++it; // pointing to 1
    auto inserted = b.Insert(it, 99);
    EXPECT_EQ(*inserted, 99);
    EXPECT_EQ(b.Size(), 5u);

    // Erase single element
    auto next = b.Erase(inserted);
    EXPECT_EQ(*next, 1);
    EXPECT_EQ(b.Size(), 4u);

    // Insert range: insert three 7s at end
    auto e = b.End();
    b.Insert(e, 3, 7);
    EXPECT_EQ(b.Back(), 7);
    EXPECT_EQ(b.Size(), 7u);

    // Erase range (last three)
    auto begin_last3 = b.End();
    begin_last3 -= 3;
    auto after = b.Erase(begin_last3, b.End());
    EXPECT_EQ(after, b.End());
    EXPECT_EQ(b.Size(), 4u);
}

TEST(WBufferCornerCases, ClearResizeAndEquality) {
    WBuffer a;
    for (uint8_t i = 0; i < 10; ++i) a.PushBack(i);

    WBuffer b = a;
    EXPECT_EQ(a, b);

    a.Clear();
    EXPECT_TRUE(a.Empty());

    b.Resize(20);
    EXPECT_EQ(b.Size(), 20u);
    b.Resize(2);
    EXPECT_EQ(b.Size(), 2u);
}

TEST(WBufferBounds, IteratorComparisons) {
    WBuffer b;
    for (uint8_t i = 0; i < 3; ++i) b.PushBack(i);
    auto it1 = b.Begin();
    auto it2 = it1;
    ++it2;
    EXPECT_TRUE(it1 < it2);
    EXPECT_TRUE(it2 > it1);
    EXPECT_TRUE(it1 <= it2);
    EXPECT_TRUE(it2 >= it1);
    EXPECT_TRUE(it1 != it2);
}

// NOLINTEND