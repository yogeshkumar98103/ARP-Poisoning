#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <net/if_arp.h>

#include "ip_address.h"
#include "mac_address.h"

#define GUARD(x, err) if(!(x)) {    \
    printf("[x] ERROR: " err "\n"); \
    exit(EXIT_FAILURE);             \
}                                   \

MacAddress get_hardware_address(int fd, const char* if_name){
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, if_name);

    GUARD(ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0, "failed to get mac address");
    printf("MAC Address: %s\n", ifr.ifr_hwaddr.sa_data);
}

int main(int argc, char** argv){
    if(argc < 4){
        // TODO: Show usage info
        return 0;
    }

    auto ip1 = IPv4Address::from_str(argv[1]);
    auto ip2 = IPv4Address::from_str(argv[2]);
    auto interface = argv[3];

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    GUARD(fd != -1, "failed to create socket");




    std::cout << "Starting ARP Poisoning" << std::endl;
    return 0;
}