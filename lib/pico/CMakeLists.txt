add_library(pico pico.cpp)

target_link_libraries(pico pico_stdlib hardware_pwm hardware_gpio)