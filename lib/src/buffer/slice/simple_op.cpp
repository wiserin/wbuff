#include <cstdint> // Copyright 2026 wiserin
#include <cstring>

#include "wbuffer/buffer.hpp"


namespace wbuffer {

uint8_t& WBufferSlice::Front() {
    return (*buffer_)[start_];
}


uint8_t WBufferSlice::Front() const {
    return (*buffer_)[start_];
}


uint8_t& WBufferSlice::Back() {
return (*buffer_)[end_];
}


uint8_t WBufferSlice::Back() const {
return (*buffer_)[end_];
}


} // namespace wbuffer


