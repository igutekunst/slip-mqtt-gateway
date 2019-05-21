cmake_minimum_required(VERSION 3.7)
project(slip_mqtt_gateway)
enable_language(CXX C)
set (CMAKE_CXX_STANDARD 11)
enable_testing()

set(CMAKE_C_STANDARD 99)
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(gtest_download)

add_library(slip_mqtt_gateway
        src/library.c
        packet-format/example_packet_handler.h
        packet-format/packet_header.h
        src/library.h)

add_executable(test_slip_mqtt_gateway tests/test_basic.cpp)
target_include_directories(slip_mqtt_gateway PRIVATE
        src
        packet-format)

target_include_directories(test_slip_mqtt_gateway PRIVATE
        src
        packet-format)
target_link_libraries(test_slip_mqtt_gateway slip_mqtt_gateway gtest gtest_main)


add_test(NAME run_unit_tests
        COMMAND test_slip_mqtt_gateway)
