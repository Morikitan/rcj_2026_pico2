add_library(line line.cpp)

target_include_directories(line PRIVATE ../action)
target_include_directories(line PRIVATE ../display)

target_link_libraries(line pico_stdlib hardware_i2c hardware_gpio hardware_uart)

#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(line PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../display/u8g2/csrc
)