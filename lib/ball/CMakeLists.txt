add_library(ball ball.cpp)

target_include_directories(ball PRIVATE ../display)
target_include_directories(ball PRIVATE ../rp2040)

target_link_libraries(ball pico_stdlib hardware_gpio hardware_i2c hardware_pio)

pico_generate_pio_header(ball ${CMAKE_CURRENT_LIST_DIR}/picoPioUart.pio)

#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(ball PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../display/u8g2/csrc
)

