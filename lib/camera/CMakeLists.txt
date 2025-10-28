add_library(camera camera.cpp)

target_link_libraries(camera pico_stdlib hardware_uart)

#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(camera PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../u8g2/csrc
)