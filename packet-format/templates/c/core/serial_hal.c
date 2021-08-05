#include "serial_hal.h"
#include "serial_hal_private.h"


int serial_send_bytes(SerialPort* p, uint8_t* buffer, size_t len) {
    if (p == NULL) return -1;
    return p->send_bytes(p, buffer, len);
}


int serial_send_byte(SerialPort* p, uint8_t byte) {
   return serial_send_bytes(p, &byte, 1);
}


int serial_recv_bytes(SerialPort*p, uint8_t* buffer, size_t len){
    if (p == NULL) return -1;
    return p->recv_bytes(p, buffer, len);
}
