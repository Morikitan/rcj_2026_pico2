add_library(motor motor.cpp)

target_link_libraries(motor pico_stdlib hardware_pwm hardware_gpio)