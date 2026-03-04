#include <algorithm> // Copyright 2026 wiserin
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


void WBuffer::Swap(WBuffer& another) noexcept {
    WBuffer tmp = std::move(another);
    another = std::move(*this);
    *this = std::move(tmp);
}


void WBuffer::Swap(WBuffer& lhs, WBuffer& rhs) noexcept {
    WBuffer tmp = std::move(lhs);
    lhs = std::move(rhs);
    rhs = std::move(tmp);
}


void WBuffer::SetAllocator(std::pmr::memory_resource* alloc) noexcept {
    resource_ = alloc;
}


size_t WBuffer::Size() const noexcept {
    return size_;
}


size_t WBuffer::Capacity() const noexcept {
    return capacity_;
}


bool WBuffer::Empty() const noexcept {
    return size_ == 0;
}


void WBuffer::Clear() {
    if (data_ != nullptr) {
        resource_->deallocate(data_, capacity_, alignof(uint8_t));
    }
    size_ = 0;
    capacity_ = 0;
    data_ = nullptr;
}


void WBuffer::SetDefaultCapacity(size_t new_capacity) noexcept {
    default_capacity = new_capacity;
}


void WBuffer::SetResizeScale(size_t new_scale) noexcept {
    resize_scale = new_scale;
}


void WBuffer::Resize(size_t new_size) {
    uint8_t* tmp = reinterpret_cast<uint8_t*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, modernize-use-auto)
        resource_->allocate(new_size, alignof(size_t)));
    
    try {
        if (data_ != nullptr) {
            memcpy(tmp, data_, std::min(new_size, size_));
            resource_->deallocate(data_, capacity_, alignof(uint8_t));
        }
        size_ = size_ < new_size ? size_ : new_size;
        capacity_ = new_size;
        data_ = tmp;
    } catch (...) {
        resource_->deallocate(tmp, new_size, alignof(uint8_t));
        throw;
    }
}


WBuffer::Iterator WBuffer::Begin() {
    return {data_, size_, 0};
}


WBuffer::Iterator WBuffer::End() {
    return {data_, size_, size_};
}

} // namespace wbuffer
