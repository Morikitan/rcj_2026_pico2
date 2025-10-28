add_library(rp2040 rp2040.cpp)

target_link_libraries(rp2040 pico_stdlib hardware_pwm hardware_gpio)
target_link_libraries(rp2040 hardware_spi hardware_pio)

pico_generate_pio_header(rp2040 ${CMAKE_CURRENT_LIST_DIR}/picoPioUart.pio)

#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(rp2040 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../u8g2/csrc
)