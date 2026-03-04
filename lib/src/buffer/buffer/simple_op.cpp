#include <algorithm> // Copyright 2026 wiserin
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


uint8_t& WBuffer::Front() {
    if (Empty()) {
        throw std::logic_error("Нельзя вернуть объект из пустого массива");
    }
    return data_[0];
}


uint8_t WBuffer::Front() const {
    if (Empty()) {
        throw std::logic_error("Нельзя вернуть объект из пустого массива");
    }
    return data_[0];
}


uint8_t& WBuffer::Back() {
    if (Empty()) {
        throw std::logic_error("Нельзя вернуть объект из пустого массива");
    }
    return data_[size_ - 1];
}


uint8_t WBuffer::Back() const {
    if (Empty()) {
        throw std::logic_error("Нельзя вернуть объект из пустого массива");
    }
    return data_[size_ - 1];
}


void WBuffer::PushBack(uint8_t byte) {
    if (size_ + 1 > capacity_) {
        Resize(std::max(capacity_ * resize_scale, default_capacity));
    }
    data_[size_] = byte;
    ++size_;
}


void WBuffer::PopBack() {
    if (Empty()) {
        return;
    }
    --size_;
}


} // namespace wbuffer


