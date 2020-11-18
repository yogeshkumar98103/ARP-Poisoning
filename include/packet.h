#ifndef __ARPOISON__PACKET
#define __ARPOISON__PACKET

#include <cinttypes>
#include "mac_address.h"
#include "ip_address.h"
#include "buffer_reader.h"
#include "buffer_writer.h"

#define ETHERNET_HW_TYPE 1
#define IPv4_PROTOCOL_TYPE 0x0800


/// Source:- https://tools.ietf.org/html/rfc6747
///
///         0        7        15       23       31
///        +--------+--------+--------+--------+
///        |       HT        |        PT       |
///        +--------+--------+--------+--------+
///        |  HAL   |  PAL   |        OP       |
///        +--------+--------+--------+--------+
///        |         S_HA (bytes 0-3)          |
///        +--------+--------+--------+--------+
///        | S_HA (bytes 4-5)|S_L32 (bytes 0-1)|
///        +--------+--------+--------+--------+
///        |S_L32 (bytes 2-3)|S_NID (bytes 0-1)|
///        +--------+--------+--------+--------+
///        |         S_NID (bytes 2-5)         |
///        +--------+--------+--------+--------+
///        |S_NID (bytes 6-7)| T_HA (bytes 0-1)|
///        +--------+--------+--------+--------+
///        |         T_HA (bytes 3-5)          |
///        +--------+--------+--------+--------+
///        |         T_L32 (bytes 0-3)         |
///        +--------+--------+--------+--------+
///        |         T_NID (bytes 0-3)         |
///        +--------+--------+--------+--------+
///        |         T_NID (bytes 4-7)         |
///        +--------+--------+--------+--------+
///
///        HT      Hardware Type (*)
///        PT      Protocol Type (*)
///        HAL     Hardware Address Length (*)
///        PAL     Protocol Address Length (uses new value 12)
///        OP      Operation Code (uses experimental value OP_EXP1=24)
///        S_HA    Sender Hardware Address (*)
///        S_L32   Sender L32  (* same as Sender IPv4 address for ARP)
///        S_NID   Sender Node Identifier (8 bytes)
///        T_HA    Target Hardware Address (*)
///        T_L32   Target L32  (* same as Target IPv4 address for ARP)
///        T_NID   Target Node Identifier (8 bytes)

struct ARPPacket {
    inline static uint16_t hardware_type = ETHERNET_HW_TYPE;
    inline static uint16_t protocol_type = IPv4_PROTOCOL_TYPE;
    inline static uint8_t  hardware_addr_len = MAC_ADDRESS_LEN;
    inline static uint8_t  protocol_addr_len = IP_ADDRESS_LEN;
    uint16_t opcode;

    MACAddress  src_mac_addr;
    IPv4Address src_ip_addr;
    MACAddress  target_mac_addr;
    IPv4Address target_ip_addr;

    ARPPacket() = default;

    ARPPacket(uint16_t opcode, MACAddress src_mac_addr, IPv4Address spoofed_ip_addr, 
              MACAddress dst_mac_addr, IPv4Address dst_ip_addr
    ){
        this->opcode = opcode;
        this->src_mac_addr = src_mac_addr;
        this->src_ip_addr = spoofed_ip_addr;
        this->target_mac_addr = dst_mac_addr;
        this->target_ip_addr = dst_ip_addr;
    }

    void write(uint8_t buffer[]){
        BufferWriter writer(buffer);
        writer.write_uint16(hardware_type);
        writer.write_uint16(protocol_type);
        writer.write_uint8(hardware_addr_len);
        writer.write_uint8(protocol_addr_len);
        writer.write_uint16(opcode);

        src_mac_addr.write(writer);
        src_ip_addr.write(writer);
        target_mac_addr.write(writer);
        target_ip_addr.write(writer);
    }

    static ARPPacket from_buffer(uint8_t buffer[]){
        ARPPacket packet;
        packet.read(buffer);
        return packet;
    }

    void read(uint8_t buffer[]){
        BufferReader reader(buffer + 6); // skip first 6 bytes
        opcode = reader.take_uint16();
        src_mac_addr.read(reader);
        src_ip_addr.read(reader);
        target_mac_addr.read(reader);
        target_ip_addr.read(reader);
    }

    static bool is_arp_packet(uint8_t buffer[]){
        BufferReader reader(buffer);
        return (
            reader.take_uint16() == hardware_type && 
            reader.take_uint16() == protocol_type && 
            reader.take_uint8()  == hardware_addr_len &&
            reader.take_uint8()  == protocol_addr_len
        );
    }
};


#endif // __ARPOISON__PACKET