#include <algorithm> // Copyright 2026 wiserin
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


void WBuffer::RightShift(size_t index, size_t count) { // NOLINT(bugprone-easily-swappable-parameters)
    if (size_ + count > capacity_) {
        Resize(std::max(capacity_ * resize_scale, default_capacity));
    }
    for (size_t i = size_; i > index; --i) {
        data_[i + count - 1] = data_[i - 1];
    }
    size_ += count;
}


void WBuffer::LeftShift(size_t index, size_t count) { // NOLINT(bugprone-easily-swappable-parameters)
    while (index + count < size_) {
        data_[index] = data_[index + count];
        ++index;
    }

    size_ -= count;
}


WBuffer::Iterator WBuffer::Insert(Iterator iter, uint8_t byte) {
    size_t index = iter - Begin();
    RightShift(index, 1);
    data_[index] = byte;
    return Begin() + index;
}


WBuffer::Iterator WBuffer::Insert(
        Iterator iter, size_t count, uint8_t byte) { // NOLINT(bugprone-easily-swappable-parameters)
    size_t index = iter - Begin();
    RightShift(index, count);

    for (size_t i = index; i < index + count; ++i) {
        data_[i] = byte;
    }
    return Begin() + index;
}


WBuffer::Iterator WBuffer::Insert(
        Iterator iter, Iterator begin, Iterator end) { // NOLINT(bugprone-easily-swappable-parameters)

    if (begin > end) {
        throw std::logic_error("Итератор начала не может быть больше итератора конца");
    }

    size_t index = iter - Begin();
    RightShift(index, end - begin);

    size_t i = index; // NOLINT(readability-identifier-length)
    while (begin != end) {
        data_[i] = *begin;
        ++begin;
        ++i;
    }
    return Begin() + index;
}


WBuffer::Iterator WBuffer::Erase(Iterator iter) {
    size_t index = iter - Begin();
    LeftShift(index, 1);
    return index < size_ ? Begin() + index : End();
}


WBuffer::Iterator WBuffer::Erase(Iterator begin, Iterator end) {
    if (begin > end) {
        throw std::logic_error("Итератор начала не может быть больше итератора конца");
    }

    size_t index = begin - Begin();

    LeftShift(index, end - begin);
    return index < size_ ? Begin() + index : End();
}


} // namespace wbuffer


