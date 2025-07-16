add_library(gyro gyro.cpp)

target_link_libraries(gyro pico_stdlib hardware_i2c hardware_gpio)