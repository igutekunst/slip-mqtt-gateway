#ifndef SERIAL_HAL_H
#define SERIAL_HAL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <stdlib.h>

typedef struct SerialPort SerialPort;


/**
 * Send len bytes to serial port p from buffer
 * @param p
 * @param buffer
 * @param len
 * @return -1 on failure, otherwise number of bytes sent
 */
int serial_send_bytes(SerialPort* p, uint8_t* buffer, size_t len);

/**
 * Send byte to serial port p
 * @param p
 * @param buffer
 * @param len
 * @return -1 on failure, otherwise number of bytes sent
 */
int serial_send_byte(SerialPort* p, uint8_t byte);


/**
 * Receive len bytes to serial port p into buffer
 * @param p
 * @param buffer
 * @param len
 * @return -1 on failure, otherwise number of bytes received
 */
int serial_recv_bytes(SerialPort*p, uint8_t* buffer, size_t len);


/**
 * Receive one byte from serial port p
 * @param p
 * @param buffer
 * @param len
 * @return -1 on failure, otherwise number of bytes received
 */
int serial_recv_byte(SerialPort*p, uint8_t* buffer);

#ifdef __cplusplus
}
#endif
#endif
