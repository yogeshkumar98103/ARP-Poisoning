#ifndef __ARPOISON__DEBUG
#define __ARPOISON__DEBUG

#include "ip_address.h"
#include "mac_address.h"

inline void print(MACAddress address){
    for(int i = 0; i < MAC_ADDRESS_LEN - 1; ++i){
        printf("%.2X:", address.octets[i]);
    }

    printf("%.2X\n", address.octets[MAC_ADDRESS_LEN - 1]);
}


inline void print(IPv4Address address){
    for(int i = 0; i < IP_ADDRESS_LEN - 1; ++i){
        printf("%d.", address.octets[i]);
    }

    printf("%d\n", address.octets[IP_ADDRESS_LEN - 1]);
}


void debug_buffer(uint8_t buffer[]){
	for(int i = 0; i < 42; ++i){
        printf("%.2X", buffer[i]);
    }
    printf("\n");
}

#endif