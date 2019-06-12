#include "gtest/gtest.h"
extern "C" {
    #include <BinaryCodec/BinaryCodec.h>
    #include <BinaryCodec/serial_hal.h>
    #include "fake_serial.h"
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
    size_t bytes_written = 0;

    union ConcentratorPacket packet;
    packet_decode_from_byte_array(&packet, ack_packet, sizeof(ack_packet), &bytes_written);
    EXPECT_EQ(packet_type, packet.packet_header.packet_type);
}


TEST(TestGeneratedEncoder, TestSanity) {
    union ConcentratorPacket packet;
    packet.packet_header.packet_type = ACK_PACKET;
    uint8_t ack_packet[100];
    packet_decode_from_byte_array(&packet, ack_packet, sizeof(ack_packet), 0);

}

TEST(TestFakeSerial, TestSanity) {
    SerialPort* p;
    int status = fake_serial_create(&p);
    ASSERT_EQ(0, status);

    uint8_t test_string[] = "This is a only a test\n";
    size_t string_len = sizeof(test_string);
    size_t written = serial_send_bytes(p, test_string, string_len);
    ASSERT_EQ(string_len, written);
}

