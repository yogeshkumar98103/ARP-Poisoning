#ifndef __ARPOISON__MAC_ADDRESS
#define __ARPOISON__MAC_ADDRESS

#include <cinttypes>
#include "buffer_reader.h"
#include "buffer_writer.h"

#define MAC_ADDRESS_LEN 6

struct MACAddress {
    uint8_t octets[MAC_ADDRESS_LEN];

    // WARNING: Assumes a valid input. No errors are handled here
    static MACAddress from_str(const char* mac_str){
        MACAddress address;

        int i = 0;
        for(int idx = 0; idx < MAC_ADDRESS_LEN; ++idx){
            int j = i + 1;
            address.octets[idx] = 0;
            while(!(mac_str[j] == '\0' || mac_str[j] == '.')){
                address.octets[idx] *= 10;
                address.octets[idx] += (mac_str[j] - '0');
                ++j;
            }

            i = j + 1;
        }

        return address;
    }

    static MACAddress get_broadcast_addr(){
        static MACAddress broadcast_addr = ([]()->MACAddress{
            MACAddress addr;
            memset(addr.octets, -1, sizeof(addr.octets)); // set all 1s
            return addr;
        })();

        return broadcast_addr;
    }

    static MACAddress from_reader(BufferReader& reader){
        MACAddress addr;
        addr.read(reader);
        return addr;
    }

    void write(BufferWriter& writer){
        for(int i = 0; i < MAC_ADDRESS_LEN; ++i){
            writer.write_uint8(octets[i]);
        }
    }

    void read(BufferReader& reader){
        for(int i = 0; i < MAC_ADDRESS_LEN; ++i){
            octets[i] = reader.take_uint8();
        }
    }

    bool operator == (const MACAddress& other) const {
        for(int i = 0; i < MAC_ADDRESS_LEN; ++i){
            if(octets[i] != other.octets[i]) return false;
        }
        return true;
    }
};

#endif // __ARPOISON__MAC_ADDRESS