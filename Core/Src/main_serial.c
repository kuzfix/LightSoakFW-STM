#include "main_serial.h"
#include "UserGPIO.h"


volatile uint8_t mainser_rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t mainser_tx_buffer[TX_BUFFER_SIZE];
volatile uint32_t mainser_rx_read_index = 0;
volatile uint32_t mainser_rx_write_index = 0;
volatile uint32_t mainser_tx_read_index = 0;
volatile uint32_t mainser_tx_write_index = 0;

//buffer for formatted out mainser_printf
char mainser_printf_buffer[MAINSER_PRINTF_BUF_LEN];

/**
 * @brief Initialize main serial communication.
 *
 * This function enables the RX interrupt for the USART.
 */
void mainser_init(void) {
  //set default baud rate
  mainser_set_baudrate(MAINSER_DEFAULT_BAUD);
  // Enable RX interrupt
  LL_USART_EnableIT_RXNE(MAINSER_UART);
}

/**
 * @brief Read data from the main serial receiver.
 *
 * This function reads one byte of data from the receiver buffer and updates the read index.
 * If the read index exceeds the buffer size, it is reset to zero.
 * Returns 0 if no data to read. Check mainser_available() first.
 *
 * @return The read data.
 */
uint8_t mainser_read(void) {
  uint8_t data;
  if(mainser_available()){
    data = mainser_rx_buffer[mainser_rx_read_index++];
    if(mainser_rx_read_index >= RX_BUFFER_SIZE) mainser_rx_read_index = 0;
    return data;
  }
  else{
    //there is no data in buffer
    return 0;
  }

}

/**
 * @brief Write data to the main serial transmitter.
 *
 * This function writes one byte of data to the transmitter buffer if there is space available.
 * It also updates the write index and enables the TXE interrupt.
 *
 * @param data The data to be written.
 * @return 1 if data is written, 0 if there is no space.
 */
uint32_t mainser_write(uint8_t data) {
  uint32_t space = mainser_tx_space();
  if(space > 0) {
    mainser_tx_buffer[mainser_tx_write_index++] = data;
    if(mainser_tx_write_index >= TX_BUFFER_SIZE) mainser_tx_write_index = 0;

    // Enable TXE interrupt
    LL_USART_EnableIT_TXE(MAINSER_UART);
    return 1;
  }
  return 0;
}

/**
 * @brief Write multiple bytes of data to the main serial transmitter.
 *
 * This function writes multiple bytes of data to the transmitter buffer if there is enough space.
 * It also updates the write index and enables the TXE interrupt.
 *
 * @param data The pointer to the data to be written.
 * @param length The length of the data.
 * @return The number of bytes written if there is enough space, 0 otherwise.
 */
uint32_t mainser_write_multi(uint8_t* data, uint32_t length) {
  uint32_t space = mainser_tx_space();
  if(space >= length) {
    for (uint32_t i = 0; i < length; i++) {
      mainser_tx_buffer[mainser_tx_write_index++] = data[i];
      if(mainser_tx_write_index >= TX_BUFFER_SIZE) mainser_tx_write_index = 0;
    }

    // Enable TXE interrupt
    LL_USART_EnableIT_TXE(MAINSER_UART);
    return length;
  }
  return 0;
}

/**
 * @brief Get the number of bytes available to read from the receiver buffer.
 *
 * This function calculates the difference between the write index and the read index to determine the number of bytes that are available to read.
 *
 * @return The number of bytes available to read.
 */
uint32_t mainser_available(void) {
  uint32_t diff;
  if(mainser_rx_write_index >= mainser_rx_read_index) {
    diff = mainser_rx_write_index - mainser_rx_read_index;
  } else {
    diff = RX_BUFFER_SIZE + mainser_rx_write_index - mainser_rx_read_index;
  }
  return diff;
}

/**
 * @brief Get the number of bytes that can be written to the transmitter buffer.
 *
 * This function calculates the difference between the write index and the read index to determine the number of bytes that can be written.
 *
 * @return The number of bytes that can be written.
 */
uint32_t mainser_tx_space(void) {
  uint32_t diff;
  //disable irq for thread safety. This was actually a problem
  __disable_irq();
  if(mainser_tx_write_index >= mainser_tx_read_index) {
    diff = TX_BUFFER_SIZE - mainser_tx_write_index + mainser_tx_read_index - 1;
  } else {
    diff = mainser_tx_read_index - mainser_tx_write_index - 1;
  }
  //re-enable irq
  __enable_irq();
  return diff;
}

/**
 * @brief Print formatted output to the main serial transmitter.
 *
 * This function formats the output string using vsnprintf, then writes the output to the transmitter buffer.
 * It waits until there is enough space in the transmitter buffer before writing.
 *
 * @param format The format string.
 * @param ... The variables to be formatted.
 */
void mainser_printf(const char* format, ...){
  uint32_t msg_len_cnt;
  va_list args;
  D1On();
  va_start(args, format);
  msg_len_cnt = 0;
  msg_len_cnt = vsnprintf(mainser_printf_buffer, MAINSER_PRINTF_BUF_LEN, format, args);
  va_end(args);
  va_end(args);
  //max length check
  if(msg_len_cnt >= TX_BUFFER_SIZE){
    //message will never fit in tx buffer, even if empty. abort
    dbg(Error, "mainser_printf: msg_len_cnt > TX_BUFFER_SIZE");
    return;
  }
  //wait for space in tx buffer
  // WARNING: this blocks code until there is space
  while(mainser_write_multi( (uint8_t*)mainser_printf_buffer, msg_len_cnt) == 0){
    //wait for space
  }
  D1Off();
}

/**
 * @brief Send null-terminated string
 *
 * @param str string pointer
 */
void mainser_send_string(const char* str){
  uint32_t n = 0;
  // send bytes up to null termination
  while(str[n] != 0){
    // wait for space in tx buffer
    while(mainser_write(str[n])==0);
    n++;
  }
}

void mainser_set_baudrate(uint32_t baudrate){
  LL_USART_Disable(MAINSER_UART);
  LL_USART_SetBaudRate(MAINSER_UART, SystemCoreClock, LL_USART_GetPrescaler(MAINSER_UART),LL_USART_OVERSAMPLING_8, baudrate);
  LL_USART_Enable(MAINSER_UART);
  mainser_printf("SETBAUD:OK\r\n");
  dbg(Warning, "mainser_set_baudrate: baudrate set to %lu\r\n", baudrate);
}
