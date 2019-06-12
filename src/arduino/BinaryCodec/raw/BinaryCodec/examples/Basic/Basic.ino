#include "BinaryCodec.h"
#include "serial_hal.h"

static union ConcentratorPacket latestRxPacket;
static uint8_t tx_packet_buffer[30];
void setup() {

}

void loop() {
    size_t bytes_encoded = 0;
    packet_encode_to_byte_array(tx_packet_buffer, &latestRxPacket, sizeof(tx_packet_buffer), &bytes_encoded);
    SerialPort* serial;
    serial_send_bytes(serial, tx_packet_buffer, bytes_encoded);

}