add_library(display display.cpp)
#add_executable(display2 u8g2_user_impl.hpp)

target_link_libraries(display pico_stdlib hardware_i2c)


target_include_directories(display PRIVATE u8g2/csrc)
#target_include_directories(u8g2_user_impl.hpp PRIVATE u8g2/csrc)