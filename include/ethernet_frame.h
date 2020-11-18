#ifndef ARPOISON_ETHERNET_FRAME
#define ARPOISON_ETHERNET_FRAME

#include <cinttypes>
#include <memory>
#include "mac_address.h"
#include "buffer_reader.h"
#include "buffer_writer.h"

#define ETHTYPE_ARP 0x0806

#define ETH_HEADER_LEN MAC_ADDRESS_LEN + MAC_ADDRESS_LEN + 2

struct EthernetFrame {
    MACAddress dst_mac_addr;
    MACAddress src_mac_addr;
    uint16_t   eth_type; 

    EthernetFrame() = default;

    EthernetFrame(
        MACAddress dst_mac_addr_, MACAddress src_mac_addr_, uint16_t eth_type_
    ):  dst_mac_addr(std::move(dst_mac_addr_)),
        src_mac_addr(std::move(src_mac_addr_)),
        eth_type(eth_type_) 
    {}

    static EthernetFrame from_buffer(uint8_t buffer[]){
        EthernetFrame packet;
        packet.read(buffer);
        return packet;
    }
    
    int write(uint8_t buffer[]){
        BufferWriter writer(buffer);
        return write(writer);
    }

    int write(BufferWriter& writer){
        dst_mac_addr.write(writer);
        src_mac_addr.write(writer);
        writer.write_uint16(eth_type);
        return ETH_HEADER_LEN;
    }

    int read(uint8_t buffer[]){
        BufferReader reader(buffer);
        return read(reader);
    }

    int read(BufferReader& reader){
        dst_mac_addr.read(reader);
        src_mac_addr.read(reader);
        eth_type = reader.take_uint16();
        return ETH_HEADER_LEN;
    }
};



#endif // ARPOISON_ETHERNET_FRAME