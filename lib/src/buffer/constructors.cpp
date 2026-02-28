#include <cstddef> // Copyright 2026 wiserin
#include <cstdint>
#include <cstring>
#include <memory_resource>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


WBuffer::WBuffer()
        : capacity_(kMinCapacity) {
    data_ = reinterpret_cast<uint8_t*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        resource_->allocate(kMinCapacity, alignof(uint8_t)));
}


WBuffer::WBuffer(size_t capacity, std::pmr::memory_resource* alloc)
        : resource_(alloc)
        , capacity_(capacity) {

    data_ = reinterpret_cast<uint8_t*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        resource_->allocate(capacity_, alignof(uint8_t)));
}


WBuffer::WBuffer(const WBuffer& another)
        : resource_(another.resource_) {
    data_ = reinterpret_cast<uint8_t*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        resource_->allocate(another.capacity_, alignof(uint8_t)));
    try {
        std::memcpy(data_, another.data_, another.size_);
        capacity_ = another.capacity_;
        size_ = another.size_;

    } catch(...) {
        resource_->deallocate(data_, capacity_, alignof(uint8_t));
        throw;
    }
}


WBuffer& WBuffer::operator=(const WBuffer& another) {
    if (this == &another) {
        return *this;
    }

    uint8_t* tmp = reinterpret_cast<uint8_t*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, modernize-use-auto)
        another.resource_->allocate(another.capacity_, alignof(uint8_t)));
    try {
        std::memcpy(tmp, another.data_, another.size_);
        resource_ = another.resource_;
        if (data_ != nullptr) {
            resource_->deallocate(data_, capacity_, alignof(uint8_t));
        }
        data_ = tmp;
        capacity_ = another.capacity_;
        size_ = another.size_;
        return *this;

    } catch(...) {
        resource_->deallocate(data_, capacity_, alignof(uint8_t));
        throw;
    }
}


WBuffer::WBuffer(WBuffer&& another) noexcept
        : data_(another.data_)
        , resource_(another.resource_)
        , size_(another.size_)
        , capacity_(another.capacity_) {
    
    another.data_ = nullptr;
    another.resource_ = nullptr;
    another.size_ = 0;
    another.capacity_ = 0;
}


WBuffer& WBuffer::operator=(WBuffer&& another) noexcept {
    if (this == &another) {
        return *this;
    }

    data_ = another.data_;
    resource_ = another.resource_;
    capacity_ = another.capacity_;
    size_ = another.size_;

    another.data_ = nullptr;
    another.resource_ = nullptr;
    another.size_ = 0;
    another.capacity_ = 0;

    return *this;
}


WBuffer::~WBuffer() {
    if (data_ != nullptr) {
        resource_->deallocate(data_, capacity_, alignof(uint8_t));
    }
}


} // namespace wbuffer


