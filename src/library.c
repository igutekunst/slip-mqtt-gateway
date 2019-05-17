#include "library.h"
#include <example_packet_handler.h>
#include <stdio.h>

static uint8_t offsets[1] = { offsetof(struct DualModeSensorPacket, adcValue) };

int hello(void) {
    printf("Hello, World!\n");
    return 0;
}