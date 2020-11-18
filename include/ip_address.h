#ifndef __ARPOISON__IP_ADDRESS
#define __ARPOISON__IP_ADDRESS

#include <cinttypes>
#include "buffer_reader.h"
#include "buffer_writer.h"

#define IP_ADDRESS_LEN 4

struct IPv4Address {
    uint8_t octets[IP_ADDRESS_LEN];

    // WARNING: Assumes a valid input. No errors are handled here
    static IPv4Address from_str(const char* ip_str){
        IPv4Address address;

        int i = 0;
        for(int idx = 0; idx < IP_ADDRESS_LEN; ++idx){
            int j = i + 1;
            address.octets[idx] = 0;
            while(!(ip_str[j] == '\0' || ip_str[j] == '.')){
                address.octets[idx] *= 10;
                address.octets[idx] += (ip_str[j] - '0');
                ++j;
            }

            i = j + 1;
        }

        return address;
    }

    static IPv4Address from_reader(BufferReader& reader){
        IPv4Address addr;
        addr.read(reader);
        return addr;
    }

    void write(BufferWriter& writer){
        for(int i = 0; i < IP_ADDRESS_LEN; ++i){
            writer.write_uint8(octets[i]);
        }
    }

    void read(BufferReader& reader){
        for(int i = 0; i < IP_ADDRESS_LEN; ++i){
            octets[i] = reader.take_uint8();
        }
    }
};

#endif // __ARPOISON__IP_ADDRESS