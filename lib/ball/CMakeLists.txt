add_library(ball ball.cpp)

target_link_libraries(ball pico_stdlib hardware_gpio hardware_i2c)

#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(ball PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../u8g2/csrc
)