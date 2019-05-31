#include "gtest/gtest.h"
extern "C" {
    #include <library.h>
    #include <packet_decoder.h>
}

TEST(TestBasic, Zero) {
    EXPECT_EQ(0, hello());
}


TEST(TestGeneratedDecoder, TestSanity) {
    uint8_t packet_type = 1;
    uint8_t source_address = 23;
    uint8_t ack_packet[] = {source_address, // Source Address
                            packet_type, // Packet Type
                            0, // Size
                            };
    uint8_t packet_size = sizeof(ack_packet);
    ack_packet[2] = packet_size;

    union ConcentratorPacket packet;
    packet_decode_from_byte_array(&packet, ack_packet, sizeof(ack_packet ));
    EXPECT_EQ(packet_type, packet.packet_header.packet_type);
    EXPECT_EQ(packet_size, packet.packet_header.size);
}


TEST(TestGeneratedEncoder, TestSanity) {
    union ConcentratorPacket packet;
    packet.packet_header.packet_type = ACK_PACKET;

}
