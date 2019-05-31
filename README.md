# Sensor Network Project

## SLIP MQTT Gateway

This is a sub-project over the IoT gateway project. This readme describes
both the larger architecture, as well as specifics ot the SLIP MQTT gateway project.


## Proposed Architecture

A 'concentrator' node is connected to Particle device via SPI or UART.
This two MCU system collectively is known as the "Gateway". 

The gateway communicates via Cell or WiFi via MQTT to an MQTT gateway such as [Adafruit IO], publishing 
updates to sensors status, either asynchronously or periodically.

### Configuration and Deployment
To deploy a new site, the concentrator node is flashed with custom/per-site firmware with sensor configuration embedded.

Initially sensor configuration is specified via a C header file, with properties simply encoded in a giant 
struct initialization.

The next workflow improvement would be to generate configuration headers via a web interface, making it 
easier and less error prone to add sensors.

Future versions could support a dynamic configuration loaded via another method, potentially OTA.

### Technical details
The "gateway" consists of a low power RF transceiver (CC1350) and a IP capable MCU with an MQTT stack.
The two communicate via a custom packet format that is encoded using [SLIP]. This translation layer
is designed to be easily portable between MCUs, the only requirement being support for ANSI C, and functions for 
sending and receiving bytes.


**Packet format**
The packet format between the IP MCU and concentrator is intentionally very simple. It maps 1:1 with the packet
format used over the air, with slight modifications used to store the length of the packet.

```C/C++
typedef struct {
    uint8_t length;
    uint8_t type;
} PacketHeader;

union CommPacket {
    struct PacketHeader header;
    struct AdcSensorPacket adcSensorPacket;
    struct DualModeSensorPacket dmSensorPacket;
};
```

## References
 * [SLIP] - A useful tool for encoding packets of an arbitrary serial link.
 * [Adafruit IO] - IoT aggregation service with MQTT support
 
[SLIP]: https://github.com/lobaro/util-slip
[Adafruit IO]: https://io.adafruit.com


## TODO
* Properly support reading/writing enumerations (size of native type might vary from network type
    - One idea here, is change the read/write routine to memcpy data to alligned area in region, and then to an assignment
      If we assume the bytes are in platform order after memcpy, then all we need to do is an assignment
      ```
      //max alligned scratch region
      uint64_t scratch;
      scratch = 0;
      memcpy(&scratch, packet, element_size);
      packet->field = *((element_type*) &scratch);

      write_uint8_t(&data_ptr, &(packet->packet_header.source_address), &bytes_remaining);
      // becomes

      scratch = 0;
      memcpy(&scratch, &data_ptr, sizeof(uint8_t);
      packet->packet_header.packet_type = *((uint8_t*) &scratch);
      ```
      This works because it will ensure the correct C type promotion/demotion happens without having to worry about
      the bytes themselves, and if an enumeration is a different size, the compiler will deal with it.. We don't need
      to know the size of all enumerations.

      The drawback is that this take quite a performance penalty to ensure aligned access. It might be better to
      do the straight memcpy in some cases, but only do the scratch in the case where the storage size is unknown, which is probably only for
      enumerations
* Verify network order is as expected
* Properly use size variable to not overflow buffers/packets
    - Use correct return type
* Make fake serial port/serial port abstraction
* Fix SLIP library and start using
