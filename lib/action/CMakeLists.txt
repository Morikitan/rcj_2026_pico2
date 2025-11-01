add_library(action action.cpp)

target_link_libraries(action pico_stdlib hardware_gpio hardware_adc)

target_include_directories(action PRIVATE ../ball)
target_include_directories(action PRIVATE ../camera)
target_include_directories(action PRIVATE ../defence)
target_include_directories(action PRIVATE ../display)
target_include_directories(action PRIVATE ../gyro)
target_include_directories(action PRIVATE ../line)
target_include_directories(action PRIVATE ../motor)


#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(action PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../display/u8g2/csrc
)