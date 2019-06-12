#include "gtest/gtest.h"
extern "C" {
    #include <BinaryCodec/serial_hal.h>
    #include "fake_serial.h"
    #include "slip.h"
}


TEST(TestSlip, TestSanity) {
   SerialPort* p;
   ASSERT_EQ(0, fake_serial_create(&p));

   uint8_t test_packet[] = "test_packet_body";
   size_t test_packet_len = sizeof(test_packet);
   slip_send_packet(test_packet, test_packet_len, p);

   ASSERT_EQ(test_packet_len + 2, fake_serial_bytes_written(p));

}



