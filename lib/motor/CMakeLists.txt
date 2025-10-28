add_library(motor motor.cpp)

target_include_directories(motor PRIVATE ../rp2040)

target_link_libraries(motor pico_stdlib hardware_pwm hardware_gpio)

#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(motor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../u8g2/csrc
)