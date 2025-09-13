#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rp2040.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "../config.hpp"

uint8_t buffer[30];
uint8_t EmptyBuffer[8];

void SPISetup(){
    gpio_init(SPI_TXpin);
    gpio_init(SPI_RXpin);
    gpio_init(SPI_SCKpin);
    gpio_init(SPI_CSpin);
    gpio_set_function(SPI_TXpin,GPIO_FUNC_SPI); 
    gpio_set_function(SPI_RXpin,GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCKpin,GPIO_FUNC_SPI);
    gpio_set_dir(SPI_CSpin,GPIO_IN);
    gpio_put(SPI_CSpin,true); //常にHIGHにしておく
    spi_init(spi1,2000000); //2MHz
    /*********************************
    SPI通信の信号
    0x01 : エンコーダー

    */
}

void UseEncoder(){
    spi_write_blocking(spi1,(uint8_t[]){0x01},1);
    while(!spi_is_readable(spi1)){
        printf("spiの待機中");
    }
    spi_read_blocking(spi1,8,buffer,8);
}

//pulse_us : 1000～2000の間。1000で静止。2000で最高速度。
void BLDCState(int pulse_us){

}