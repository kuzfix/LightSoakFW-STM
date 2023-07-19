//
// Created by Matej Planinšek on 06/07/2023.
//

#ifndef SCI_H
#define SCI_H
// ----------- Include other modules (for public) -------------

// Include low-level USART support.
#include "stm32g4xx_ll_usart.h"

// We will need the following LL functions:
//		- void LL_USART_TransmitData8(USART_TypeDef *USARTx, uint8_t Value)
//		- uint8_t LL_USART_ReceiveData8(USART_TypeDef *USARTx)
//		- uint32_t LL_USART_IsActiveFlag_TXE_TXFNF(USART_TypeDef *USARTx)
//		- uint32_t LL_USART_IsActiveFlag_RXNE_RXFNE (USART_TypeDef * USARTx)


// From the functions above we can see that the handle structure requires only one parameter
// that specifies which USARTx we will use.




// -------------------- Public definitions --------------------

// Here we publicly define the SCI return codes that will help us with error
// reports for functions dealing with receiving or transmitting data.
typedef enum { SCI_ERROR=-1, SCI_NO_ERROR=0} SCI_rtrn_codes_t;





// ---------------- Public function prototypes ----------------

void SCI_init(void);

void SCI_send_char(char c);
void SCI_send_byte(uint8_t data);

uint8_t SCI_is_data_waiting(void);

SCI_rtrn_codes_t SCI_read_char(char *c);
SCI_rtrn_codes_t SCI_read_byte(uint8_t *data);


void SCI_send_string(char *str);
void SCI_send_bytes(uint8_t *data, uint32_t size);

int SCI_send_string_IT(char *str);
int SCI_send_bytes_IT(uint8_t *data, uint32_t size);

void SCI_receive_char_Callback(void);
void SCI_transmit_char_Callback(void);



void SCI_demo_Hello_world(void);
void SCI_demo_echo_with_polling(void);
void SCI_demo_Hello_world_printf(void);

void SCI_demo_receive_with_interrupts(void);
void SCI_demo_transmit_with_interrupts(void);
void SCI_demo_echo_with_interrupts(void);

#endif //SCI_H