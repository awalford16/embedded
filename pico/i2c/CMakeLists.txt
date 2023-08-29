cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico-isquaredc)

pico_sdk_init()

add_executable(isquaredc
    main.c
)

target_link_libraries(isquaredc
    pico_stdlib
    hardware_i2c
)

pico_enable_stdio_usb(isquaredc 1)
pico_enable_stdio_uart(isquaredc 0)

pico_add_extra_outputs(isquaredc)
