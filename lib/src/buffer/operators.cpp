#include <cstddef> // Copyright 2026 wiserin
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


uint8_t& WBuffer::operator[](size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Индекс превышает длинну массива");
    }
    return data_[index];
}


uint8_t WBuffer::operator[](size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Индекс превышает длинну массива");
    }
    return data_[index];
}


bool WBuffer::operator==(const WBuffer& another) const noexcept {
    return data_ == another.data_;
}


bool WBuffer::operator!=(const WBuffer& another) const noexcept {
    return !(*this == another);
}

} // namespace wbuffer
