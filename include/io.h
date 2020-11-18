#ifndef __ARPOISON__IO
#define __ARPOISON__IO

#include "buffer_reader.h"
#include "buffer_writer.h"

template <typename payload_t>
int write(BufferWriter& writer, payload_t& head){
	return head.write(writer);
}

template <typename payload_t, typename... payload_u>
int write(BufferWriter& writer, payload_t& head, payload_u&&... tail){
    int size = head.write(writer);
    return size + write(writer, std::forward<payload_u>(tail)...);
}

template <typename... payload_u>
int write(uint8_t buffer[], payload_u&&... payload){
    BufferWriter writer(buffer);
    return write(writer, std::forward<payload_u>(payload)...);
}

template <typename payload_t>
int read(BufferReader& reader, payload_t& head){
    return head.read(reader);
}

template <typename payload_t, typename... payload_u>
int read(BufferReader& reader, payload_t& head, payload_u&&... tail){
    int size = head.read(reader);
    return size + read(reader, std::forward<payload_u>(tail)...);
}

template <typename... payload_u>
int read(uint8_t buffer[], payload_u&&... payload){
    BufferReader reader(buffer);
    return read(reader, std::forward<payload_u>(payload)...);
}


#endif // __ARPOISON__IO