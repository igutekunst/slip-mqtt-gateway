#ifndef FAKE_SERIAL_H
#define FAKE_SERIAL_H
#include <BinaryCodec/serial_hal.h>
#include <stdlib.h>

int fake_serial_create(SerialPort **serial_port);

size_t fake_serial_bytes_written(SerialPort *serial_port);
#endif