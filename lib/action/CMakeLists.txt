add_library(action action.cpp)

target_link_libraries(action pico_stdlib hardware_gpio hardware_adc)