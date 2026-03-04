#include <cstddef> // Copyright 2026 wiserin
#include <cstring>

#include "wbuffer/buffer.hpp"


namespace wbuffer {


WBufferSlice::WBufferSlice(WBuffer* buffer, size_t start, size_t end) noexcept
        : buffer_(buffer)
        , start_(start)
        , end_(end) {}


WBufferSlice::WBufferSlice(WBufferSlice&& another) noexcept 
        : buffer_(another.buffer_)
        , start_(another.start_)
        , end_(another.end_) {

    another.buffer_ = nullptr;
}


WBufferSlice& WBufferSlice::operator=(WBufferSlice&& another) noexcept {
    if (this == &another) {
        return *this;
    }

    buffer_ = another.buffer_;
    start_ = another.start_;
    end_ = another.end_;

    another.buffer_ = nullptr;

    return *this;
}


} // namespace wbuffer
