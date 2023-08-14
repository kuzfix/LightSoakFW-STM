#ifndef LIGHTSOAKFW_STM_MAIN_SERIAL_H
#define LIGHTSOAKFW_STM_MAIN_SERIAL_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_usart.h"
#include <stdarg.h>
#include <stdio.h>
#include "debug.h"

#define MAINSER_UART USART3  // change this to the USART instance you are using
#define RX_BUFFER_SIZE 128  // adjust this as needed
#define TX_BUFFER_SIZE 512  // adjust this as needed

//buffer size for mainser_printf
#define MAINSER_PRINTF_BUF_LEN 256

extern volatile uint8_t mainser_rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t mainser_tx_buffer[TX_BUFFER_SIZE];
extern volatile uint32_t mainser_rx_read_index;
extern volatile uint32_t mainser_rx_write_index;
extern volatile uint32_t mainser_tx_read_index;
extern volatile uint32_t mainser_tx_write_index;

void mainser_init(void);
uint8_t mainser_read(void);
uint32_t mainser_write(uint8_t data);
uint32_t mainser_write_multi(uint8_t* data, uint32_t length);
void mainser_printf(const char* format, ...);
uint32_t mainser_available(void);
uint32_t mainser_tx_space(void);
void mainser_send_string(const char* str);

#endif //LIGHTSOAKFW_STM_MAIN_SERIAL_H