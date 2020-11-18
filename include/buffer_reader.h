#ifndef __ARPOISON__BUFFER_READER
#define __ARPOISON__BUFFER_READER

#include <cinttypes>

class BufferReader {
    uint8_t* buffer;

public:

    explicit BufferReader(uint8_t* buffer_): buffer(buffer_) {}

    uint8_t* get_buffer(){
        return buffer;
    }

    uint8_t read_uint8(){
        return *(buffer);
    }

    uint16_t read_uint16(){
        return  (static_cast<uint16_t>(buffer[0]) << 8) |
                (static_cast<uint16_t>(buffer[1])     ) ;
    }

    uint32_t read_uint32(){
        return  (static_cast<uint32_t>(buffer[0]) << 24) |
                (static_cast<uint32_t>(buffer[1]) << 16) |
                (static_cast<uint32_t>(buffer[2]) << 8 ) |
                (static_cast<uint32_t>(buffer[3])      ) ;
    }

    uint64_t read_uint64(){
        return  (static_cast<uint64_t>(buffer[0]) << 56) |
                (static_cast<uint64_t>(buffer[1]) << 48) |
                (static_cast<uint64_t>(buffer[2]) << 40) |
                (static_cast<uint64_t>(buffer[3]) << 32) |
                (static_cast<uint64_t>(buffer[4]) << 24) |
                (static_cast<uint64_t>(buffer[5]) << 16) |
                (static_cast<uint64_t>(buffer[6]) << 8 ) |
                (static_cast<uint64_t>(buffer[7])      ) ;
    }

    uint8_t take_uint8(){
        uint8_t value = read_uint8();
        buffer += 1;
        return value;
    }

    uint16_t take_uint16(){
        uint16_t value = read_uint16();
        buffer += 2;
        return value;
    }

    uint32_t take_uint32(){
        uint32_t value = read_uint32();
        buffer += 4;
        return value;
    }

    uint64_t take_uint64(){
        uint64_t value = read_uint64();
        buffer += 8;
        return value;
    }
};

#endif // __ARPOISON__BUFFER_READER