#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void BallSetup();
void UseBallSensor();
void picoPioUartTx_program_putc2(unsigned char c, bool even_parity);
unsigned char picoPioUartRx_program_getc2(bool even_parity,bool* parity_check);

#ifdef __cplusplus
}
#endif