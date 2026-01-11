#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void RP2040Setup();
void UseEncoder();
void BLDCState(int pulse_us);
void UseGyroSensor();
void picoPioUartTx_program_putc(unsigned char c, bool even_parity);
unsigned char picoPioUartRx_program_getc(bool even_parity,bool* parity_check);
void Called(unsigned int gpio, unsigned long events);

#ifdef __cplusplus
}
#endif