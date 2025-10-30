add_library(defence defence.cpp)

pico_set_program_name(defence "defence")
pico_set_program_version(defence "0.1")

target_include_directories(defence PRIVATE ../motor)
target_include_directories(defence PRIVATE ../action)
target_include_directories(defence PRIVATE ../gyro)
target_include_directories(defence PRIVATE ../ball)
target_include_directories(defence PRIVATE ../line)
target_include_directories(defence PRIVATE ../camera)
target_include_directories(defence PRIVATE ../pico)

target_link_libraries(defence pico_stdlib hardware_pwm hardware_gpio)

#display.hppの中のu8g2ライブラリを有効にする
target_include_directories(defence PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../display/u8g2/csrc
)