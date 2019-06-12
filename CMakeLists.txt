cmake_minimum_required(VERSION 3.7)
project(BinaryCodec VERSION 0.0.4)
enable_language(CXX C)
set (CMAKE_CXX_STANDARD 11)
enable_testing()

set(CMAKE_C_STANDARD 99)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(gtest_download)

set(PACKET_TEMPLATES
        src/templates/packet_decoder.c.mako
        )

add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/include/BinaryCodec/BinaryCodecHeader.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND python3 packet-format/generate_headers.py
                        --yaml-file src/packet.yaml
                        --output-dir ${CMAKE_CURRENT_SOURCE_DIR}/include/BinaryCodec
                        --templates src/templates/BinaryCodecHeader.h.mako
        DEPENDS
              src/packet.yaml
              src/templates/BinaryCodec.c.mako
        COMMENT "generating packet header files"
)

add_custom_command(
        OUTPUT  generated_src/BinaryCodec.c
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND python3 packet-format/generate_headers.py
                        --yaml-file src/packet.yaml
                        --output-dir ${CMAKE_CURRENT_BINARY_DIR}/generated_src
                        --templates src/templates/BinaryCodec.c.mako
        DEPENDS
              src/packet.yaml
              src/templates/BinaryCodec.c.mako
        COMMENT "generating code c file"
)

add_library(binary_codec
        src/slip.c
        src/slip.h
        src/serial_hal.c
        generated_src/BinaryCodec.c
        include/BinaryCodec/BinaryCodec.h
        include/BinaryCodec/BinaryCodecHeader.h
        )

target_include_directories(binary_codec
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/BinaryCodec>
            ${CMAKE_CURRENT_BINARY_DIR}/generated_src
        )

add_custom_command(
        TARGET binary_codec
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/src/arduino/BinaryCodec
        $<TARGET_FILE_DIR:binary_codec>
       )

add_executable(test_binary_codec tests/test_basic.cpp tests/fake_serial.c tests/test_slip.cpp)

target_include_directories(test_binary_codec PRIVATE

        ${CMAKE_CURRENT_SOURCE_DIR}/src
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/BinaryCodec>
        ${CMAKE_CURRENT_BINARY_DIR}/generated_src
)


target_link_libraries(test_binary_codec
        PUBLIC
            binary_codec
        PRIVATE
            gtest gtest_main
        )



# Install

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/BinaryCodec)

install(TARGETS binary_codec
        EXPORT binary_codec-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

#This is required so that the exported target has the name BinaryCodec and not jsonutils
set_target_properties(binary_codec PROPERTIES EXPORT_NAME BinaryCodec)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#Export the targets to a script
install(EXPORT binary_codec-targets
        FILE
        BinaryCodecTargets.cmake
        DESTINATION
        ${INSTALL_CONFIGDIR}
        )

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/BinaryCodecConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/BinaryCodecConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/BinaryCodecConfig.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BinaryCodecConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/BinaryCodecConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
        )

##############################################
## Exporting from the build tree

export(EXPORT binary_codec-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/BinaryCodecTargets.cmake )

#Register package in user's package registry
export(PACKAGE BinaryCodec)


# Export Arduino Library

set(ARDUINO_LIBRARY_SOURCE_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/generated_src/BinaryCodec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/slip.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/slip.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/serial_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/serial_hal_private.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BinaryCodec/BinaryCodec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BinaryCodec/BinaryCodecHeader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BinaryCodec/serial_hal.h
)

install(
        FILES ${ARDUINO_LIBRARY_SOURCE_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/arduino/libraries/BinaryCodec/src
)

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/arduino/BinaryCodec/raw/BinaryCodec
        DESTINATION ${CMAKE_INSTALL_PREFIX}/arduino/libraries
        )

install(
        DIRECTORY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/arduino/BinaryCodec/templates/BinaryCodec/README.md.in
        ${CMAKE_INSTALL_PREFIX}/arduino/libraries/BinaryCodec/README.md
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/arduino/BinaryCodec/templates/BinaryCodec/library.properties.in
        ${CMAKE_INSTALL_PREFIX}/arduino/libraries/BinaryCodec/library.properties
)

