# Add executable. Default name is the project name, version 0.1

add_executable(main main.cpp )

pico_set_program_name(main "main")
pico_set_program_version(main "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(main 0)
pico_enable_stdio_usb(main 0)

target_include_directories(main PRIVATE ../lib)

# Add the standard library to the build
target_link_libraries(main pico_stdlib)
target_link_libraries(main action ball camera display)
target_link_libraries(main gyro line motor pico)

# Add the standard include files to the build
target_include_directories(main PRIVATE ${CMAKE_CURRENT_LIST_DIR})

#target_include_directories(main PRIVATE
#    ${CMAKE_CURRENT_LIST_DIR}/ssd1306
#)
target_include_directories(main PRIVATE ../lib/display/u8g2/csrc)

pico_add_extra_outputs(main)
