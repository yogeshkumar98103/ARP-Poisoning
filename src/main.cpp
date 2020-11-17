#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <net/if_arp.h>

#include "ip_address.h"
#include "mac_address.h"
#include "debug.h"

#define GUARD(x, err) if(!(x)) {    \
    printf("[x] ERROR: " err "\n"); \
    exit(EXIT_FAILURE);             \
}                                   \

MACAddress get_hardware_address(int fd, const char* if_name){
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ-1);

    GUARD(ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0, "failed to get mac address");
    
    unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    MACAddress address;

    for(int i = 0; i < MAC_ADDRESS_LEN; ++i){
        address.octets[i] = mac[i];
    }

    return address;    
}

int main(int argc, char** argv){
    if(argc < 4){

        // TODO: Show usage info
        printf("Expected 5 arguments got %d", argc);
        return 0;
    }

    auto ip1 = IPv4Address::from_str(argv[1]);
    auto ip2 = IPv4Address::from_str(argv[2]);
    auto interface = argv[3];
    printf("Interface: %s\n", interface);

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    GUARD(fd != -1, "failed to create socket");

    MACAddress mac = get_hardware_address(fd, interface);
    PRINT_MAC_ADDRESS(mac.octets);

    // close(fd);

    std::cout << "Starting ARP Poisoning" << std::endl;
    return 0;
}