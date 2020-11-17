#include <cinttypes>
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
};