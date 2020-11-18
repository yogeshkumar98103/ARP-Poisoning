#ifndef __ARPOISON__BUFFER_WRITER
#define __ARPOISON__BUFFER_WRITER

#include <cinttypes>

class BufferWriter {
    uint8_t* buffer;

public:

    explicit BufferWriter(uint8_t* buffer_): buffer(buffer_) {}

    uint8_t* get_buffer(){
        return buffer;
    }

    void write_uint8(uint8_t value){
        buffer[0] = value;
        ++buffer;
    }

    void write_uint16(uint16_t value){
        buffer[0] = (value >> 8);
        buffer[1] = (value & 0xff);
        buffer += 2;
    }

    void write_uint32(uint32_t value){
        buffer[0] = value >> 24;
        buffer[1] = ((value >> 16) & 0xff);
        buffer[2] = ((value >> 8 ) & 0xff);
        buffer[3] = (value & 0xff);
        buffer += 4;
    }

    void write_uint64(uint64_t value){
        buffer[0] = value >> 56;
        buffer[1] = ((value >> 48) & 0xff);
        buffer[2] = ((value >> 40) & 0xff);
        buffer[3] = ((value >> 32) & 0xff);
        buffer[4] = ((value >> 24) & 0xff);
        buffer[5] = ((value >> 16) & 0xff);
        buffer[6] = ((value >> 8 ) & 0xff);
        buffer[7] = (value & 0xff);
        buffer += 8;
    }
};

#endif // __ARPOISON__BUFFER_WRITER