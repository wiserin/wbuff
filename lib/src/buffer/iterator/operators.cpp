#include <cstddef> // Copyright 2026 wiserin
#include <cstdint>
#include <stdexcept>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


WBuffer::Iterator& WBuffer::Iterator::operator++() {
    if (position_ + 1 > size_) {
        throw std::out_of_range("Позиция итератора не может быть > size");
    }
    ++position_;
    return *this;
}


WBuffer::Iterator WBuffer::Iterator::operator++(int) {
    if (position_ + 1 > size_) {
        throw std::out_of_range("Позиция итератора не может быть > size");
    }
    WBuffer::Iterator tmp = *this;
    ++position_;
    return tmp;
}


WBuffer::Iterator& WBuffer::Iterator::operator--() {
    if (position_ - 1 < 0) {
        throw std::out_of_range("Позиция итератора не может быть < 0");
    }
    --position_;
    return *this;
}


WBuffer::Iterator WBuffer::Iterator::operator--(int) {
    if (position_ - 1 < 0) {
        throw std::out_of_range("Позиция итератора не может быть < 0");
    }
    WBuffer::Iterator tmp = *this;
    --position_;
    return tmp;
}


WBuffer::Iterator& WBuffer::Iterator::operator+=(size_t index) {
    if (position_ + index > size_) {
        throw std::out_of_range("Позиция итератора не может быть > size");
    }
    position_ += index;
    return *this;
}


WBuffer::Iterator WBuffer::Iterator::operator+(size_t index) const {
    if (position_ + index > size_) {
        throw std::out_of_range("Позиция итератора не может быть > size");
    }

    WBuffer::Iterator tmp(ptr_, size_, position_ + index);

    return tmp;
}


WBuffer::Iterator& WBuffer::Iterator::operator-=(size_t index) {
    if (position_ - index < 0) {
        throw std::out_of_range("Позиция итератора не может быть < 0");
    }
    position_ -= index;
    return *this;
}


WBuffer::Iterator WBuffer::Iterator::operator-(size_t index) const {
    if (position_ - index < 0) {
        throw std::out_of_range("Позиция итератора не может быть < 0");
    }
    
    WBuffer::Iterator tmp(ptr_, size_, position_ - index);
    
    return tmp;
}


size_t WBuffer::Iterator::operator-(const Iterator& another) const {
    return position_ - another.position_;
}


bool WBuffer::Iterator::operator==(const Iterator& another) const {
    return position_ == another.position_;
}


bool WBuffer::Iterator::operator!=(const Iterator& another) const {
    return !(*this == another);
}


bool WBuffer::Iterator::operator>=(const Iterator& another) const {
    return position_ >= another.position_;
}


bool WBuffer::Iterator::operator<=(const Iterator& another) const {
    return position_ <= another.position_;
}



bool WBuffer::Iterator::operator<(const Iterator& another) const {
    return position_ < another.position_;
}


bool WBuffer::Iterator::operator>(const Iterator& another) const {
    return position_ > another.position_;
}


uint8_t& WBuffer::Iterator::operator*() const {
    return ptr_[position_];
}


uint8_t& WBuffer::Iterator::operator[](size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Выход за границы массива"); 
    }
    return ptr_[index];
}
 

} // namespace wbuffer


