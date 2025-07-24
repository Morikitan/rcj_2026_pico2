add_library(display display.cpp)

target_include_directories(display PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(u8g2)

target_link_libraries(display u8g2 hardware_i2c pico_stdlib hardware_gpio)

target_include_directories(display PRIVATE u8g2/csrc)