#pragma once // Copyright 2026 wiserin
#include <cstddef>
#include <cstdint>
#include <memory_resource>


namespace wbuffer {
constexpr size_t kMinCapacity = 128; // NOLINT(readability-identifier-naming)
constexpr size_t kResizeScale = 2; // NOLINT(readability-identifier-naming)


class WBuffer {
    inline static size_t min_capacity = kMinCapacity;
    inline static size_t resize_scale = kResizeScale;

    uint8_t* data_ = nullptr;
    std::pmr::memory_resource* resource_ = nullptr;

    size_t size_ = 0;
    size_t capacity_ = 0;

    void RightShift(size_t index, size_t count);
    void LeftShift(size_t index, size_t count);

 public:
    class Iterator {
        uint8_t* ptr_ = nullptr;
        size_t size_ = 0;
        size_t position_ = 0;

     public:
        Iterator(uint8_t* ptr, size_t size, size_t position);
        
        Iterator(const Iterator& another) = default;
        Iterator& operator=(const Iterator& another) = default;
        Iterator(Iterator&& another) = default;
        Iterator& operator=(Iterator&& another) = default;

        Iterator& operator++();
        [[nodiscard]] Iterator operator++(int);
        Iterator& operator--();
        [[nodiscard]] Iterator operator--(int);
        Iterator& operator+=(size_t index);
        [[nodiscard]] Iterator operator+(size_t index) const;
        Iterator& operator-=(size_t index);
        [[nodiscard]] Iterator operator-(size_t index) const;

        size_t operator-(Iterator another) const;

        [[nodiscard]] bool operator==(Iterator another) const;
        [[nodiscard]] bool operator!=(Iterator another) const;
        [[nodiscard]] bool operator>=(Iterator another) const;
        [[nodiscard]] bool operator<=(Iterator another) const;
        [[nodiscard]] bool operator<(Iterator another) const;
        [[nodiscard]] bool operator>(Iterator another) const;

        [[nodiscard]] uint8_t& operator*() const;

        [[nodiscard]] uint8_t& operator[](size_t index) const;

        ~Iterator() = default;
    };

    static void SetDefaultCapacity(size_t new_capacity) noexcept;
    static void SetResizeScale(size_t new_scale) noexcept;

    WBuffer() = default;

    WBuffer(const WBuffer& another);
    WBuffer& operator=(const WBuffer& another);
    WBuffer(WBuffer&& another) noexcept;
    WBuffer& operator=(WBuffer&& another) noexcept;

    WBuffer(size_t capacity, std::pmr::memory_resource* alloc);

    [[nodiscard]] uint8_t& operator[](size_t index);
    [[nodiscard]] uint8_t operator[](size_t index) const;

    [[nodiscard]] bool operator==(const WBuffer& another) const noexcept;
    [[nodiscard]] bool operator!=(const WBuffer& another) const noexcept;

    void Swap(WBuffer& another) noexcept;
    static void Swap(WBuffer& lhs, WBuffer& rhs) noexcept;

    Iterator Insert(Iterator iter, uint8_t byte);
    Iterator Insert(Iterator iter, size_t count, uint8_t byte);
    Iterator Insert(Iterator iter, Iterator begin, Iterator end);

    [[nodiscard]] uint8_t& Front();
    [[nodiscard]] uint8_t Front() const;

    [[nodiscard]] uint8_t& Back();
    [[nodiscard]] uint8_t Back() const;

    void PushBack(uint8_t byte);
    void PopBack();

    Iterator Erase(Iterator iter);
    Iterator Erase(Iterator begin, Iterator end);

    [[nodiscard]] Iterator Begin();
    [[nodiscard]] Iterator End();

    [[nodiscard]] size_t Size() const noexcept;
    [[nodiscard]] size_t Capacity() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    void Clear();

    void Resize(size_t new_size);

    ~WBuffer();
};

} // namespace wbuffer
