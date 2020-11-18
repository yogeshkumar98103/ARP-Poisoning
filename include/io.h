#ifndef __ARPOISON__IO
#define __ARPOISON__IO

#include "buffer_reader.h"
#include "buffer_writer.h"

template <typename payload_t>
int write(BufferWriter& writer, payload_t head){
    return head.write(writer);
}

template <typename payload_t, typename... payload_u>
int write(BufferWriter& writer, payload_t& head, payload_u&&... tail){
    return head.write(writer) + write(writer, std::forward<payload_u>(tail)...);
}

template <typename... payload_u>
int write(uint8_t buffer[], payload_u&&... payload){
    BufferWriter writer(buffer);
    return write(writer, std::forward<payload_u>(payload)...);
}

template <typename payload_t>
int read(BufferWriter& writer, payload_t head){
    return head.read(writer);
}

template <typename payload_t, typename... payload_u>
int read(BufferWriter& writer, payload_t& head, payload_u&&... tail){
    return head.read(writer) + read(writer, std::forward<payload_u>(tail)...);
}

template <typename... payload_u>
int read(uint8_t buffer[], payload_u&&... payload){
    BufferWriter writer(buffer);
    return read(writer, std::forward<payload_u>(payload)...);
}


#endif // __ARPOISON__IO