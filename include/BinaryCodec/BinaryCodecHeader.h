#ifndef PACKET_HEADER_H
#define PACKET_HEADER_H
/**
* This file is auto-generated from /Users/igutek/Documents/Projects/Kevin/Firmware/slip-mqtt-gateway/src/templates/BinaryCodecHeader.h.mako. Please
* edit the source template if possible and then re-run  generate_headers.py.
*/

#include <stdint.h>
#include <stdbool.h>


enum PacketType {
    ACK_PACKET,
    ADC_SENSOR_PACKET,
    DUAL_MODE_SENSOR_PACKET
};

struct PacketHeader { 
    uint8_t source_address;
    enum PacketType packet_type;
};

struct AckPacket { 
    struct PacketHeader packet_header;
};

struct AdcSensorPacket { 
    struct PacketHeader packet_header;
    uint16_t adc_value;
};

struct DualModeSensorPacket { 
    struct PacketHeader packet_header;
    uint16_t adc_value;
    uint16_t battery_percentage;
    uint32_t time;
    uint8_t button;
    bool conc_led_toggle;
};

union ConcentratorPacket { 
    struct PacketHeader packet_header;
    struct AckPacket ack_packet;
    struct AdcSensorPacket adc_sensor_packet;
    struct DualModeSensorPacket dual_mode_sensor_packet;
};


#endif







