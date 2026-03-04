#include <cstddef> // Copyright 2026 wiserin
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


uint8_t& WBufferSlice::operator[](size_t index) {
    size_t relative_idx = index + start_;
    if (relative_idx > end_) {
        throw std::out_of_range("Индекс превышает длинну среза");
    }
    return (*buffer_)[relative_idx];
}


uint8_t WBufferSlice::operator[](size_t index) const {
    size_t relative_idx = index + start_;
    if (relative_idx > end_) {
        throw std::out_of_range("Индекс превышает длинну среза");
    }
    return (*buffer_)[relative_idx];
}


bool WBufferSlice::operator==(const BaseWBuffer& another) const {
    const WBufferSlice* another_ptr = dynamic_cast<const WBufferSlice*>(&another);
    if (another_ptr == nullptr) {
        return false;
    }
    return *this == *another_ptr;
}


bool WBufferSlice::operator==(const WBufferSlice& another) const {
    if (buffer_->Size() != another.Size()) {
        return false;
    }
    for (int i = 0; i < buffer_->Size(); ++i) {
        if ((*buffer_)[i] != another[i]) {
            return false;
        }
    }
    return true;
}


bool WBufferSlice::operator!=(const BaseWBuffer& another) const {
    const WBufferSlice* another_ptr = dynamic_cast<const WBufferSlice*>(&another);
    if (another_ptr == nullptr) {
        return true;
    }
    return *this != *another_ptr;
}


bool WBufferSlice::operator!=(const WBufferSlice& another) const {
    return !(*this == another);
}

} // namespace wbuffer
