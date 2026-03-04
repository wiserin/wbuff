#include <cstddef> // Copyright 2026 wiserin
#include <memory_resource>

#include "wbuffer/buffer.hpp"


namespace wbuffer {

std::pmr::memory_resource* WBufferSlice::GetAlloc() const noexcept {
    return buffer_->GetAlloc();
}


size_t WBufferSlice::Size() const noexcept {
    return (end_ - start_) + 1;
}


size_t WBufferSlice::Capacity() const noexcept {
    return Size();
}


WBufferSlice::Iterator WBufferSlice::Begin() {
    return buffer_->Begin() + start_;
}


WBufferSlice::Iterator WBufferSlice::End() {
    return buffer_->Begin() + end_ + 1;
}

} // namespace wbuffer
