#pragma once // Copyright 2026 wiserin
#include <cstddef>
#include <cstdint>
#include <memory_resource>


namespace wbuffer {
constexpr size_t kDefaultCapacity = 128; // NOLINT(readability-identifier-naming)
constexpr size_t kResizeScale = 2; // NOLINT(readability-identifier-naming)


class BaseWBuffer { // NOLINT(cppcoreguidelines-special-member-functions)
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

    [[nodiscard]] virtual uint8_t& operator[](size_t index) = 0;
    [[nodiscard]] virtual uint8_t operator[](size_t index) const = 0;;

    [[nodiscard]] virtual bool operator==(const BaseWBuffer& another) const = 0;
    [[nodiscard]] virtual bool operator!=(const BaseWBuffer& another) const = 0;

    [[nodiscard]] virtual uint8_t& Front() = 0;
    [[nodiscard]] virtual uint8_t Front() const = 0;

    [[nodiscard]] virtual uint8_t& Back() = 0;
    [[nodiscard]] virtual uint8_t Back() const = 0;

    [[nodiscard]] virtual Iterator Begin() = 0;
    [[nodiscard]] virtual Iterator End() = 0;

    [[nodiscard]] virtual size_t Size() const noexcept = 0;
    [[nodiscard]] virtual size_t Capacity() const noexcept = 0;
    [[nodiscard]] virtual bool Empty() const noexcept = 0;

    virtual ~BaseWBuffer() = default;
};

class WBufferSlice;

class WBuffer : public BaseWBuffer {
    inline static size_t default_capacity = kDefaultCapacity;
    inline static size_t resize_scale = kResizeScale;

    uint8_t* data_ = nullptr;
    std::pmr::memory_resource* resource_ = nullptr;

    size_t size_ = 0;
    size_t capacity_ = 0;

    void RightShift(size_t index, size_t count);
    void LeftShift(size_t index, size_t count);

 public:

    static void SetDefaultCapacity(size_t new_capacity) noexcept;
    static void SetResizeScale(size_t new_scale) noexcept;

    WBuffer() = default;

    WBuffer(const WBuffer& another);
    WBuffer& operator=(const WBuffer& another);
    WBuffer(WBuffer&& another) noexcept;
    WBuffer& operator=(WBuffer&& another) noexcept;

    WBuffer(const WBufferSlice& another);
    WBuffer& operator=(const WBufferSlice& another);

    WBuffer(std::pmr::memory_resource* alloc);
    WBuffer(size_t capacity, std::pmr::memory_resource* alloc);

    [[nodiscard]] uint8_t& operator[](size_t index) override;
    [[nodiscard]] uint8_t operator[](size_t index) const override;

    [[nodiscard]] bool operator==(const BaseWBuffer& another) const override;
    [[nodiscard]] bool operator!=(const BaseWBuffer& another) const override;

    [[nodiscard]] bool operator==(const WBuffer& another) const;
    [[nodiscard]] bool operator!=(const WBuffer& another) const;

    void SetAllocator(std::pmr::memory_resource* alloc) noexcept;

    void Swap(WBuffer& another) noexcept;
    static void Swap(WBuffer& lhs, WBuffer& rhs) noexcept;

    Iterator Insert(Iterator iter, uint8_t byte);
    Iterator Insert(Iterator iter, size_t count, uint8_t byte);
    Iterator Insert(Iterator iter, Iterator begin, Iterator end);

    [[nodiscard]] uint8_t& Front() override;
    [[nodiscard]] uint8_t Front() const override;

    [[nodiscard]] uint8_t& Back() override;
    [[nodiscard]] uint8_t Back() const override;

    void PushBack(uint8_t byte);
    void PopBack();

    Iterator Erase(Iterator iter);
    Iterator Erase(Iterator begin, Iterator end);

    [[nodiscard]] Iterator Begin() override;
    [[nodiscard]] Iterator End() override;

    [[nodiscard]] size_t Size() const noexcept override;
    [[nodiscard]] size_t Capacity() const noexcept override;
    [[nodiscard]] bool Empty() const noexcept override;
    [[nodiscard]] std::pmr::memory_resource* GetAlloc() const noexcept;
    void Clear();

    void Resize(size_t new_size);

    ~WBuffer() override;
};


class WBufferSlice : public BaseWBuffer { // NOLINT(cppcoreguidelines-pro-type-member-init)
    WBuffer* buffer_;

    size_t start_ = 0;
    size_t end_ = 0;

 public:
    WBufferSlice() = default;

    WBufferSlice(WBuffer* buffer, size_t start, size_t end);

    WBufferSlice(const WBufferSlice& another) = delete;
    WBufferSlice& operator=(const WBufferSlice& another) = delete;
    WBufferSlice(WBufferSlice&& another) noexcept;
    WBufferSlice& operator=(WBufferSlice&& another) noexcept;


    [[nodiscard]] uint8_t& operator[](size_t index) override;
    [[nodiscard]] uint8_t operator[](size_t index) const override;

    [[nodiscard]] bool operator==(const BaseWBuffer& another) const override;
    [[nodiscard]] bool operator!=(const BaseWBuffer& another) const override;

    [[nodiscard]] bool operator==(const WBuffer& another) const;
    [[nodiscard]] bool operator!=(const WBuffer& another) const;

    [[nodiscard]] uint8_t& Front() override;
    [[nodiscard]] uint8_t Front() const override;

    [[nodiscard]] uint8_t& Back() override;
    [[nodiscard]] uint8_t Back() const override;

    [[nodiscard]] Iterator Begin() override;
    [[nodiscard]] Iterator End() override;

    [[nodiscard]] size_t Size() const noexcept override;
    [[nodiscard]] size_t Capacity() const noexcept override;
    [[nodiscard]] bool Empty() const noexcept override;

    ~WBufferSlice() override = default;
};


} // namespace wbuffer
