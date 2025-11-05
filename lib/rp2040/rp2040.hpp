#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void RP2040Setup();
void UseEncoder();
void BLDCState();
void picoPioUartTx_program_putc(unsigned char c, bool even_parity);
unsigned char picoPioUartRx_program_getc(bool even_parity,bool* parity_check);
void ChangeMode(uint gpio, uint32_t events);

#ifdef __cplusplus
}
#endif