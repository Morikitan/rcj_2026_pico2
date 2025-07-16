add_library(line line.cpp)

target_link_libraries(line pico_stdlib hardware_i2c hardware_gpio)