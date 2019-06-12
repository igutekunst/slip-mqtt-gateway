#ifndef SERIAL_HAL_PRIVATE_H
#define SERIAL_HAL_PRIVATE_H
#include <stdlib.h>
#include <stdint.h>

struct SerialPort{
    size_t (*send_bytes)(struct SerialPort* p, uint8_t* buffer, size_t len);
    size_t (*recv_bytes)(struct SerialPort*p, uint8_t* buffer, size_t len);
    void * _private;
};

#endif