# Binary Code Generation Tool
## Overview 
This library consists of tools for generating binary codecs/serializers/deserializers.

Given a YAML file, this library will generate code for supported platforms for serializing and
deserializing binary structures.

## More Details

Currently only ANSI C is supported as a language. 


###  Library Structure

This library is designed to run on multiple targets: Desktop (Posix), Arduino,
Arduino like (Particle), and TI RTOS projects, specifically the CC13xx wireless MCUs.

The library also may target multiple languages and runtimes. 

Examples:
* ANSI C struct to/from byte array
* ANSI C struct to/from FILE like object (block and non-blocking)
* Wireshark disector
* Python wrapper for ANSI C on platforms supporting pyserial
* Pure python implementation using pyserial

Each of these will potentially be implemented a bit different. Right now the structure I'm playing with is:

* Top level python package
* ANSI C python package

The ANSI c python package will depend on features in the top level package. The top level package will 
contain code to generate a CMake project. Each support platform will leave in a python plugin (package), that can be
installed separately (inspired by sphinx).

To use the library, you'll define a new folder with some kind of packet.yaml, and maybe some additional metadata, such
as enabled plugins.

Then you'll run some command, like "generate_codec_cmake", which will create a cmake project with a 
top level CMakeLists.txt, with sub-directories for each plugin.

Each plugin in turn will have a metadata component, a python component, and a CMake component. The python component might be empty.

The metadata component will specify a list of templates that need to be rendered by the top level project. Some
will be pretty much straight copy operations into a per-plugin directory, including a per-plugin CMakeLists.txt

The Per plugin CMakeLists.txt will depend on certain CMake variables and functions being present to generate its
output in however way it wants.

We could potentially flip the dependencies around a slight bit. In this case, there is a top-level
cmake project that provides functions for invoking the python template rendering, and maybe a bit
more. Each plugin can in turn depend on this, which allows you to compile each one separately,
potentially with different toolchain files.

After much deliberation, I think it makes the most sense to have the python generate create fully independent 
CMake projects. There's not need to add extra complexity with a CMake dependency graph.

One interesting piece: For all the C targets at least, there is a common set of 
C files, such as `serial_hal.c`,`slip.c`, and maybe a few more.

I think the sub-projects, or plugins need some way to pull in these common *templates*, so 
some kind of template search path may be in order.

Maybe you have a metadata.yaml
```yaml
inherited_templates:
  - destination: src
    files:
      - ${base}/serial_hal.c
      - ${base}/slip.c
  - destination: include
    files:
      - ${base}/serial_hal.h
      - ${base}/slip.h
  - destination: ${root}
    files:
      - BinaryCodec
    
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
      
* Properly use size variable to not overflow buffers/packets
* Re-organize to generate arduino libraries
* Make some more unit tests


