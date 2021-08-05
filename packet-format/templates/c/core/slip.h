/**
 * slip.c
 *
 *  Created on: 19.12.2016
 *  Author: Tobias
 *  Downloaded from: https://github.com/lobaro/util-slip
 *  License: https://github.com/lobaro/util-slip/blob/master/LICENSE
 */

#ifndef SRC_UTIL_SLIP_SLIP_H_
#define SRC_UTIL_SLIP_SLIP_H_
#include "serial_hal.h"

/* SLIP special character codes
 */
#define SLIP_END             ((uint8_t)0300)    /* indicates end of packet */
#define SLIP_ESC             ((uint8_t)0333)    /* indicates byte stuffing */
#define SLIP_ESC_END         ((uint8_t)0334)    /* ESC ESC_END means END data byte */
#define SLIP_ESC_ESC         ((uint8_t)0335)    /* ESC ESC_ESC means ESC data byte */
#define SLIP_ERROR           (-1)


/* slip_recv_packet: reads a packet from buf into the buffer
 * located at "p". If more than len bytes are received, the
 * packet will be truncated.
 * Returns the number of bytes stored in the buffer.
 * Returns 0 if the buffer does not contain a full packet.
 */
int slip_read_packet(uint8_t *p, int len, SerialPort* serial);
void slip_send_packet(uint8_t *p, int len, SerialPort* serial);
void slip_encode(const uint8_t* p, int len, SerialPort* serial);


#endif /* SRC_UTIL_SLIP_SLIP_H_ */