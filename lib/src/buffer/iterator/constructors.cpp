#include <cstddef> // Copyright 2026 wiserin
#include <cstdint>
#include <stdexcept>

#include "wbuffer/buffer.hpp"


namespace wbuffer {

BaseWBuffer::Iterator::Iterator(uint8_t* ptr, size_t size, size_t position) //NOLINT(bugprone-easily-swappable-parameters)
        : ptr_(ptr)
        , size_(size) {
    
    if (position > size_) {
        throw std::out_of_range("Позиция итератора не может быть > size");
    }
    position_ = position;
}

} // namespace wbuffer


