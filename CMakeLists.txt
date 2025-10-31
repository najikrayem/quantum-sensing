cmake_minimum_required(VERSION 3.13...3.27)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)


# initialize the Raspberry Pi Pico SDK
project(quantum-sensing)
pico_sdk_init()


# Add subdirectories
add_subdirectory(transmitter)


# Add Executables
add_executable(transmitter_main transmitter/main.c)


# Link Libraries
target_link_libraries(transmitter_main pico_stdlib signal_generator_lib dbh_12_lib manchester_encoding_lib transmitter_lib)

# enable usb output, disable uart output
pico_enable_stdio_usb(transmitter_main 1)
pico_enable_stdio_uart(transmitter_main 0)


# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(transmitter_main)


