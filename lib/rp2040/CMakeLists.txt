add_library(rp2040 rp2040.cpp)

target_link_libraries(rp2040 pico_stdlib hardware_pwm hardware_gpio)
target_link_libraries(rp2040 hardware_spi)
