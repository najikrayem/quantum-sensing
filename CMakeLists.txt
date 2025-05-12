cmake_minimum_required(VERSION 3.13...3.27)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(tx-rx-fusion
    tx-rx-fusion.c
)

# add_executable(main
#     main.c
# )

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(tx-rx-fusion pico_stdlib hardware_spi pico_multicore)

# target_link_libraries(main
#     pico_stdlib
#     hardware_spi)

# enable usb output, disable uart output
pico_enable_stdio_usb(tx-rx-fusion 1)
pico_enable_stdio_uart(tx-rx-fusion 0)

# # enable usb output, disable uart output
# pico_enable_stdio_usb(main 1)
# pico_enable_stdio_uart(main 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(tx-rx-fusion)
#pico_add_extra_outputs(main)

