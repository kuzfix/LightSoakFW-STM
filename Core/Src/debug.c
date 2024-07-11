//
// Created by Matej Planinšek on 12/07/2023.
//
// Follow usage instructions in debug.h

#include "debug.h"
#include <stdarg.h>
#include "usart.h"
#include "UserGPIO.h"

void dbg(DebugLevel level, const char* format, ...) {
  static char buffer[DEBUG_BUFFER_SIZE];
  uint32_t msg_len_cnt;
  va_list args;
  va_start(args, format);

  // Wait for the UART to be ready to transmit.
  while(HAL_UART_GetState(&hlpuart1) != HAL_UART_STATE_READY);
  msg_len_cnt = 0;
  switch (level) {
    case Error:
#if DEBUG_LEVEL >= DBG_ERROR
      strncpy(buffer, DBG_MSG_ERROR, sizeof(buffer));
      vsnprintf(buffer + strlen(DBG_MSG_ERROR), sizeof(buffer) - strlen(DBG_MSG_ERROR), format, args);
      msg_len_cnt = 0;
      while(buffer[msg_len_cnt] != 0) msg_len_cnt++;
      break;
#endif
    case Warning:
#if DEBUG_LEVEL >= DBG_WARNING
      strncpy(buffer, DBG_MSG_WARNING, sizeof(buffer));
        vsnprintf(buffer + strlen(DBG_MSG_WARNING), sizeof(buffer) - strlen(DBG_MSG_WARNING), format, args);
        // Add your function to send buffer out to hardware here.
      msg_len_cnt = 0;
      while(buffer[msg_len_cnt] != 0) msg_len_cnt++;
      break;
#endif
    case Debug:
#if DEBUG_LEVEL >= DBG_ALL
      strncpy(buffer, DBG_MSG_DEBUG, sizeof(buffer));
        vsnprintf(buffer + strlen(DBG_MSG_DEBUG), sizeof(buffer) - strlen(DBG_MSG_DEBUG), format, args);
        // Add your function to send buffer out to hardware here.
      msg_len_cnt = 0;
      while(buffer[msg_len_cnt] != 0) msg_len_cnt++;
      break;
#endif
    default:
      break;
  }

  //start uart transmission
  HAL_UART_Transmit_DMA(DEBUG_UART_HANDLE, (uint8_t *)(buffer), msg_len_cnt);

  va_end(args);
}
