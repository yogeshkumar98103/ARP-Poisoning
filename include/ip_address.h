#include <cinttypes>

struct IPv4Address {
    uint8_t octets[4];

    // WARNING: Assumes a valid input. No errors are handled here
    static IPv4Address from_str(const char* ip_str){
        IPv4Address address;

        int i = 0;
        for(int idx = 0; idx < 4; ++idx){
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
};