#include <stdio.h>
#include "line.hpp"
#include "pico/stdlib.h"
#include "../config.hpp"
#include "hardware/uart.h"

uint8_t buffer[32];

void LineSensorSetup(){
    gpio_init(LineTXpin);
    gpio_init(LineRXpin);
    gpio_set_function(LineTXpin,GPIO_FUNC_UART);
    gpio_set_function(LineRXpin,GPIO_FUNC_UART);
    uart_init(LineUart,115200);
}

void UseLineSensor(){
    
}