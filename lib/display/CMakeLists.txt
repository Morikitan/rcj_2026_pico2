add_library(display display.cpp)

target_link_libraries(display pico_stdlib hardware_gpio hardware_i2c)