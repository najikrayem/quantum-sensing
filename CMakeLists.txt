cmake_minimum_required(VERSION 3.13...3.27)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
project(my_project)


# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


# Add subdirectories
add_subdirectory(src)


# Add executables
add_executable(2D_antenna src/2D_antenna.c)
# add_executable(spool_antenna src/spool_antenna.c)
# add_executable(tx-rx-fusion src/tx-rx-fusion.c)


# Add libraries
target_link_libraries(2D_antenna pico_stdlib signal_generator_lib dbh_12_lib manchester_encoding_lib transmitter_lib)
# target_link_libraries(tx-rx-fusion pico_stdlib hardware_spi pico_multicore)
# target_link_libraries(spool_antenna pico_stdlib hardware_pwm)



# enable usb output, disable uart output
pico_enable_stdio_usb(2D_antenna 1)
pico_enable_stdio_uart(2D_antenna 0)
# pico_enable_stdio_usb(tx-rx-fusion 1)
# pico_enable_stdio_uart(tx-rx-fusion 0)
# pico_enable_stdio_usb(spool_antenna 1)
# pico_enable_stdio_uart(spool_antenna 0)
# pico_enable_stdio_usb(main 1)
# pico_enable_stdio_uart(main 0)


# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(2D_antenna)
# pico_add_extra_outputs(tx-rx-fusion)
# pico_add_extra_outputs(spool_antenna)
# pico_add_extra_outputs(main)


