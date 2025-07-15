add_library(line line.cpp)

pico_set_program_name(line "line")
pico_set_program_version(line "0.1")

target_link_libraries(line pico_stdlib hardware_i2c hardware_gpio)