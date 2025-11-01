#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rp2040.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "picoPioUart.pio.h"
#include "../config.hpp"

uint8_t buffer[30];
uint8_t EmptyBuffer[8];

PIO pio;
uint sm_rx;
uint sm_tx;
uint offset;
uint offset2;
bool parity_check;
unsigned char encoderData[8];

void OldSPISetup(){
    gpio_init(SPI_TXpin);
    gpio_init(SPI_RXpin);
    gpio_init(SPI_SCKpin);
    gpio_init(SPI_CSpin);
    gpio_set_function(SPI_TXpin,GPIO_FUNC_SPI); 
    gpio_set_function(SPI_RXpin,GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCKpin,GPIO_FUNC_SPI);
    gpio_set_dir(SPI_CSpin,GPIO_OUT);
    gpio_put(SPI_CSpin,false); //常にLOWにしておく
    spi_init(spi1,2000000); //2MHz
    /*********************************
    SPI通信の信号
    0x01 : エンコーダー

    */
}

//pioをつかったUARTの初期設定
void RP2040Setup(){
    /*********************************
    UART通信の信号
    0x24 : エンコーダー
    0x48 : BLDC
    */
    pio = pio0;

    sm_rx = 0;

    offset = pio_add_program(pio, &picoPioUartRx_program);
    picoPioUartRx_program_init(pio, sm_rx, offset, (uint)SPI_RXpin, SERIAL_BAUD);

    // 使うSMを指定します(送信と受信では別のSMを使う)
    sm_tx = 1;

    offset2 = pio_add_program(pio, &picoPioUartTx_program);
    picoPioUartTx_program_init(pio, sm_tx, offset2, (uint)SPI_TXpin, SERIAL_BAUD);
}

void OldUseEncoder(){
    spi_write_blocking(spi1,(uint8_t[]){0x01},1);
    while(!spi_is_readable(spi1)){
        tight_loop_contents();
        //printf("spiの待機中");
    }
    spi_read_blocking(spi1,8,buffer,8);
    printf("%d %d %d %d %d %d %d %d \n",buffer[0],buffer[1]
    ,buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
}

//pulse_us : 1000～2000の間。1000で静止。2000で最高速度。
void BLDCState(int pulse_us){
    picoPioUartTx_program_putc(0x48,true);
    picoPioUartTx_program_putc((unsigned char)(pulse_us / 256),true);
    picoPioUartTx_program_putc((unsigned char)(pulse_us % 256),true);
}

void UseEncoder(){
    picoPioUartTx_program_putc(0x24,true);
    for(int i = 0;i < 8;i++){
        encoderData[i] = picoPioUartRx_program_getc(true,&parity_check);
    }
    for(int i = 0;i < 4;i++){
        int motornumber = encoderData[i*2] >> 5;
        if(encoderData[i*2] >> 4 & 0x01 == 0x01) isMotorClockWise[i] = true;
        motorFrequency[i] = ((uint16_t)(encoderData[i*2] & 0x0F) << 8 | (uint16_t)encoderData[i*2 + 1]) / 100.0;    
    }
}

//UART(シリアル通信)で送信する関数
//
//data : 送るデータ(uint8_t型)
//even_parity : 偶数か奇数のどちらになるようにパリティを付加するか。trueで偶数。falseで奇数。
void picoPioUartTx_program_putc(unsigned char data, bool even_parity) {
    uint32_t byte = (uint32_t)data;
    uint8_t parity = 0;
    for (int i = 0; i < 8; i++) {
        parity ^= byte & 0x1;
        byte >>= 1;
    }
    byte = (uint32_t)data;
    if (parity) {
        if (even_parity) {
            byte |= 0x100;  // 偶数になるようにパリティを付加します
        }
    } else {
        if (!even_parity) {
            byte |= 0x100;  // 奇数になるようにパリティを付加します
        }
    }
    pio_sm_put_blocking(pio, sm_tx, (uint32_t)byte);  // TX FIFOへputします
}

//UART(シリアル通信)で受信する関数
//
//
//even_parity : 偶数か奇数のどちらになるようにパリティを付加されているか。trueで偶数。falseで奇数。
//parity_check : パリティビットの結果。正しいならtrue。違ったらfalseで、例外処理を用意する。データがなくてもfalseになる。
unsigned char picoPioUartRx_program_getc(bool even_parity,bool* parity_check) {
    // if(pio_sm_is_rx_fifo_empty(pio, sm_rx)){
        // *parity_check = false;
        // return 0;
    // }else{
     while (pio_sm_is_rx_fifo_empty(pio, sm_rx)) tight_loop_contents();

    uint32_t c32 = pio_sm_get(pio, sm_rx);

    c32 >>= 23;
    //パリティビットの検証をする
    bool real_parity = (c32 & 0x100) != 0;
    uint8_t byte = c32 & 0xff;

    uint8_t pcheck = 0;
    for (int i = 0; i < 8; i++) {
        pcheck ^= byte & 0x1;
        byte >>= 1;
    }

    *parity_check = (pcheck == real_parity);

    return (unsigned char)(c32 & 0xff);
    // }
}

