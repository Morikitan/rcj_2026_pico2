add_library(camera camera.cpp)

target_link_libraries(camera pico_stdlib hardware_uart)