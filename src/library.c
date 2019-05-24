#include "library.h"
#include <packet_header.h>
#include <stdio.h>


int hello(void) {
    printf("Hello, World!\n");

    union ConcentratorPacket packet = {
        .dual_mode_sensor_packet = {
            .packet_header = {
                .packet_type = DUAL_MODE_SENSOR_PACKET
            },
            .adc_value = 15,
            .time = 40,
            .button = false,
            .battery_percentage = 100
        }

    };
    return 0;
}