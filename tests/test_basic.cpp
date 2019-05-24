#include "gtest/gtest.h"
extern "C" {
    #include <library.h>
    #include <packet_decoder.h>
}

TEST(TestBasic, Zero) {
    EXPECT_EQ(0, hello());
}

TEST(TestGeneratedDecoder, TestAck) {
    uint8_t ack_packet[] = {0, // Source Address
                            0, 0, 0, 0, // Packet Type
                            17, // Size
                            };
    union ConcentratorPacket packet;
    packet_decode_from_byte_array(&packet, ack_packet, sizeof(ack_packet ));
    EXPECT_EQ(0, packet.packet_header.packet_type);
    EXPECT_EQ(17, packet.packet_header.size);

}
