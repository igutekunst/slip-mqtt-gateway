#include "serial_hal_private.h"
#include "fake_serial.h"
#include <BinaryCodec/serial_hal.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t* rx_buffer;
    uint8_t* tx_buffer;
    size_t bytes_written;
} FakeSerial;

size_t fake_serial_recv_bytes(SerialPort *port, uint8_t* buffer, size_t len) {
    return -1;

}


size_t fake_serial_send_bytes(SerialPort *port, uint8_t* buffer, size_t len) {
    FakeSerial* s = (FakeSerial*) port->_private;
    s->bytes_written += len;

#if (FAKE_SERIAL_VERBOSE)
    printf("tx %d bytes: ", len);
    for (size_t i = 0; i  < len; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n");

    for (size_t i = 0; i  < len; i++) {
        printf("TX: '%c': %02x\n", buffer[i], buffer[i]);
    }
    printf("done\n");
#endif
    return len;
}

int fake_serial_create(SerialPort **serial_port) {
    if (serial_port == NULL) return -1;

    SerialPort* new_serial_port = (SerialPort*) malloc(sizeof(SerialPort));
    if (new_serial_port == NULL)  return -1;

    FakeSerial* fake_serial = (FakeSerial*) malloc(sizeof(FakeSerial));
    if (fake_serial == NULL) {
        free(new_serial_port);
        return -1;
    }

    fake_serial->bytes_written = 0;

    new_serial_port->send_bytes = fake_serial_send_bytes;
    new_serial_port->recv_bytes = fake_serial_recv_bytes;
    new_serial_port->_private = (void*) fake_serial;
    *serial_port = new_serial_port;
    return 0;
}


size_t fake_serial_bytes_written(SerialPort *serial_port) {
    FakeSerial* s = (FakeSerial*) serial_port->_private;
    return s->bytes_written;
}
