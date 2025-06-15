# dbh-12
add_library(dbh_12_lib STATIC)
target_sources(dbh_12_lib PRIVATE dbh-12.c)
target_link_libraries(dbh_12_lib pico_stdlib hardware_pwm hardware_clocks m)


# signal_generator
add_library(signal_generator_lib STATIC)
target_sources(signal_generator_lib PRIVATE signal_generator.c)
target_link_libraries(signal_generator_lib pico_stdlib m)

# manchester_encoding_lib
add_library(manchester_encoding_lib STATIC)
target_sources(manchester_encoding_lib PRIVATE manchester_encoding.c)
target_link_libraries(manchester_encoding_lib pico_stdlib)

# transmitter_lib
add_library(transmitter_lib STATIC)
target_sources(transmitter_lib PRIVATE transmitter.c)
target_link_libraries(transmitter_lib pico_stdlib dbh_12_lib signal_generator_lib)

