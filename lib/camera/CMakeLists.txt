add_library(camera camera.cpp)

pico_set_program_name(camera "camera")
pico_set_program_version(camera "0.1")

target_link_libraries(camera pico_stdlib hardware_uart)