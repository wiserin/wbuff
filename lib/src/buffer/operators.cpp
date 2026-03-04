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


bool WBuffer::operator==(const WBuffer& another) const {
    if (size_ != another.size_) {
        return false;
    }
    for (int i = 0; i < size_; ++i) {
        if (data_[i] != another.data_[i]) {
            return false;
        }
    }
    return true;
}


bool WBuffer::operator!=(const WBuffer& another) const {
    return !(*this == another);
}

} // namespace wbuffer
