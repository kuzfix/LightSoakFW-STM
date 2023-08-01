//
// Created by Matej Planinšek on 06/07/2023.
//
/*
 * SCI.c
 *
 *  Created on: Jan 21, 2022
 *      Author: Gasper
 */

/* **************** MODULE DESCRIPTION *************************

This module implements the system Serial Communication Interface (SCI)
which provides a serial connection to the PC via the USART periphery.

Two types of data transmission over the SCI is supported:

	1. text transfer (used for transmitting text messages),

	2. binary data transfer (used for transmitting binary data).


Additionally, two types of "send" functions are available:

	1. for sending a single character or a single byte of data

	2. for sending a string of characters or a number of bytes


NOTE: in order for this module to work properly, you must ensure
that the appropriate UART interface is initialized at low-level.

************************************************************* */





// ----------- Include other modules (for private) -------------
#include "SCI.h"

#include <stdio.h>			// add printf() functionality
#include "buf.h"			// add buffer to SCI



// ---------------------- Private definitions ------------------


// ------ SCI definitions -------

// Define the handle structure that we will use to operate the SCI.
// We only need to specify which USART instance is going to be used.
typedef struct
{
    USART_TypeDef *USART;
} SCI_handle_t;


// And now define the private SCI handle variable.
SCI_handle_t SCI;



// ------ printf() customization definitions -----

// Declare the customized _write() function used by printf() to print strings.
int _write(int file, char *ptr, int len);


// ------ SCI receive (RX) buffer definitions -------

// We must also prepare the data structures that will implement the SCI buffer.



// And define the data structures required to implement the circular buffer:
uint8_t SCI_RX_buffer[SCI_RX_BUF_LEN];	// the buffer data array
buf_handle_t SCI_RX_buf_handle;			// the buffer handle structure



// ------ SCI transmit (TX) buffer definitions -------

// We must also prepare the data structures that will implement the SCI buffer.



// And define the data structures required to implement the circular buffer:
uint8_t SCI_TX_buffer[SCI_TX_BUF_LEN];	// the buffer data array
buf_handle_t SCI_TX_buf_handle;			// the buffer handle structure


//buffer for formatted out SCI_formatted
char SCI_printf_buffer[SCI_PRINTF_BUF_LEN];






// -------------- Public function implementations --------------

// Function SCI_init() takes care of the SCI "handle" structure initialization,
// printf() buffer customization, RX and TX system buffer initialization and
// interrupt management.
void SCI_init(void)
{
    // 0. Make sure that the appropriate low-level hardware is already initialized!

    // The initialization of the UART interface  is done by the
    // auto-generated CubeMX code within the MX_USARTx_UART_Init() function!



    // 1. Specify which USARTx instance is going to be used to implement the SCI.
    SCI.USART = USART3;

    // 2. printf() customization
    // Initialize the printf() buffer to zero length
    // to achieve the best response time.
    setvbuf(stdout, NULL, _IONBF, 0);

    // Note: use setvbuf to specify what kind of buffering you want for the file or stream.
    // Use the predefined mode argument:
    // _IONBF -> Do not use a buffer: send output directly to the host system for the file or
    // stream identified by fp.


    // 3. Initialize the SCI buffers (RX and TX)
    BUF_init( &SCI_RX_buf_handle, SCI_RX_buffer, SCI_RX_BUF_LEN);
    BUF_init( &SCI_TX_buf_handle, SCI_TX_buffer, SCI_TX_BUF_LEN);


    // 4. Initialize the USART interrupts
    LL_USART_EnableIT_RXNE_RXFNE(SCI.USART);

    // Mind that the RXNE IRQ is always on, while the TX IRQ should be enabled
    // only if there is some data to be transmitted.

}



// ------ Sending one byte -------

// Function SCI_send_char() sends one character via the SCI (useful when working with text strings).
void SCI_send_char(char c)
{
    // Make sure that the transmitter is ready to receive another character by waiting in the loop
    while( ! LL_USART_IsActiveFlag_TXE_TXFNF(SCI.USART) );

    // and then send the character.
    LL_USART_TransmitData8(SCI.USART, c);

}


// Function SCI_send_byte() sends one one byte via the SCI (useful when working with binary data).
void SCI_send_byte(uint8_t data)
{
    // Make sure that the transmitter is ready to receive another byte of data by waiting in the loop
    while( ! LL_USART_IsActiveFlag_TXE_TXFNF(SCI.USART) );

    // and then send the byte.
    LL_USART_TransmitData8(SCI.USART, data);
}




// ------ Sending more bytes -------

// Function SCI_send_string() sends a string message via the SCI.
void SCI_send_string(char *str)
{
    uint32_t i = 0;

    // Send the entire string, that is send character by character
    // until null value "0" is reached.
    while(str[i] != 0)
    {
        SCI_send_char( str[i] );
        i++;
    }


}

// Function SCI_send_bytes() sends several bytes from a given location.
// The input parameters provide the data location and the size of data
// to be sent.
void SCI_send_bytes(uint8_t *data, uint32_t size)
{
    for(uint32_t i=0; i < size ; i++ )
    {
        SCI_send_byte( data[i] );
    }
}




// ------ Sending more bytes using interrupts -------


int SCI_send_string_IT(char *str)
{
    buf_rtrn_codes_t result;
    uint32_t num_bytes_sent = 0;
    uint8_t c;

    // Store all of the string characters into the SCI TX buffer first
    while( str[num_bytes_sent] != 0)		// keep storing characters until null character
    {
        result = BUF_store_byte( &SCI_TX_buf_handle, (uint8_t)str[num_bytes_sent] );
        if (result == BUFFER_FULL) break;
        num_bytes_sent++;
    }

    // If there is at least 1 character to be sent,
    if ( num_bytes_sent > 0 )
    {
        // check if we can send the first character right away
        if ( LL_USART_IsActiveFlag_TC(SCI.USART) )		// transfer complete
        {
            // send the first character
            BUF_get_byte( &SCI_TX_buf_handle, &c );
            LL_USART_TransmitData8(SCI.USART, c);	// Note: writing to USART TDR register clears the TC flag.
        }


        // Also enable the USART TX complete interrupt request
        // to let the IRQ routine send the rest of the characters.
        LL_USART_EnableIT_TC(SCI.USART);

    }
    return num_bytes_sent;
}




int SCI_send_bytes_IT(uint8_t *data, uint32_t size)
{
    // temporary one-byte data storage
    buf_rtrn_codes_t result;
    uint8_t	d;
    uint32_t num_bytes_sent = 0;

    // store all the data in the TX buffer
    for(; num_bytes_sent < size ; num_bytes_sent++ )
    {
        result = BUF_store_byte( &SCI_TX_buf_handle, data[num_bytes_sent] );
        if (result == BUFFER_FULL) break;
    }


    // If there is at least 1 byte to be sent,
    if ( size > 0 )
    {
        // check if we can send the first byte right away
        if ( LL_USART_IsActiveFlag_TC(SCI.USART) )		// transfer complete
        {
            // send the first byte
            BUF_get_byte( &SCI_TX_buf_handle, &d );
            LL_USART_TransmitData8(SCI.USART, d);	// Note: writing to USART TDR register clears the TC flag.
        }


        // Also enable the USART TX complete interrupt request
        // to let the IRQ routine send the rest of the bytes.
        LL_USART_EnableIT_TC(SCI.USART);

    }
    return num_bytes_sent;
}













// ------ Reading one byte -------


// Function SCI_is_data_waiting_in_reg() checks if there is received data waiting in the UART interface.
uint8_t SCI_is_data_waiting_in_reg(void)
{
    return ( (uint8_t) LL_USART_IsActiveFlag_RXNE_RXFNE (SCI.USART));

    // Note that explicit type-casting is required on the return value.
}


// Function SCI_read_char_from_reg() reads one character from the UART interface and
// stores it in the location specified by the function input argument *c.
// Function returns error the following codes: SCI_NO_ERROR, SCI_ERROR;
SCI_rtrn_codes_t SCI_read_char_from_reg(char *c)
{
    if (SCI_is_data_waiting_in_reg() )
    {
        // save the received character in the provide destination
        *c = (char) LL_USART_ReceiveData8(SCI.USART);

        return SCI_NO_ERROR;	// data was available -> no error
    }
    else
    {
        return SCI_ERROR;		// no data was available -> error!
    }


}



// Function SCI_read_char_from_reg() reads one byte from the UART interface and
// stores it in the location specified by the function input argument *data.
// Function returns error the following codes: SCI_NO_ERROR, SCI_ERROR;
SCI_rtrn_codes_t SCI_read_byte_from_reg(uint8_t *data)
{
    if (SCI_is_data_waiting_in_reg() )
    {
        // save the received character in the provide destination
        *data = (uint8_t) LL_USART_ReceiveData8(SCI.USART);

        return SCI_NO_ERROR;	// data was available -> no error
    }
    else
    {
        return SCI_ERROR;		// no data // data was available -> error!
    }

}


uint8_t SCI_available(void){
  return BUF_get_data_size(&SCI_RX_buf_handle) > 0;
}
void SCI_write(char write_data){
  uint8_t data_byte = (uint8_t)write_data;
  SCI_send_bytes_IT(&data_byte, 1);
}
char SCI_read(void){
  uint8_t data_byte;
  if(BUF_get_data_size( &SCI_RX_buf_handle) > 0){
    BUF_get_byte(&SCI_RX_buf_handle, &data_byte);
    return (char)data_byte;
  }
  return 0;
}








// ------- Interrupt callbacks  ---------

void SCI_receive_char_Callback(void)
{
    // Store the newly received byte into the SCI RX buffer.
    BUF_store_byte( &SCI_RX_buf_handle, (char) LL_USART_ReceiveData8(SCI.USART) );
}



void SCI_transmit_char_Callback(void)
{
    uint8_t c;

    // try to get the next byte from the SCI TX buffer
    if ( BUF_get_byte( &SCI_TX_buf_handle, &c ) == BUFFER_OK )
        LL_USART_TransmitData8(SCI.USART, c);	// and send it via USART

    // check if there is no data left in the buffer
    if ( BUF_get_data_size( &SCI_TX_buf_handle ) == 0)
        LL_USART_DisableIT_TC(SCI.USART);	// and disable the TC IRQ in that case

}











// ------- Test functions ---------

// A simple "Hello world!" demo using the SCI_send_string() function.
void SCI_demo_Hello_world(void)
{

    char text[32] = "Hello world!!!\n";

    SCI_send_string(text);

    HAL_Delay(1000);
}


// A simple "Hello world!" demo using the customized printf() function.
void SCI_demo_Hello_world_printf(void)
{

    for(int i=0; i<100; i++)
    {
        // Demonstrate the SCI formatted text functionality added by the
        // customization of the printf() function.
        printf("%2d : Hello printf() world!\n", i);

        HAL_Delay(1000);
    }

}




// An SCI "echo test" implemented using the "polling technique".
void SCI_demo_echo_with_polling(void)
{

    // variable for the received character
    char c;


    // auxiliary variables and parameters that are used in simulating the "CPU busy" situation
    float a_0 = 0.5;
    float a_n;
    float sum = 0;

#define N_MAX		500


    // In an endless loop,
    while(1)
    {

        // poll for a new received character.
        if (SCI_read_char_from_reg(&c) == SCI_NO_ERROR )
            SCI_send_char(c);	// and send it back immediately (i.e. echo).


        // After that, simulate the "microcontroller busy" situation by
        // calculating the geometric sum of N_MAX elements.
        // Vary the N_MAX parameter and see the effect on the echo functionality.
        // Also try commenting this section.

        a_n = a_0;
        for(uint32_t n=0; n<N_MAX; n++)
        {
            sum = sum + a_n;
            a_n = a_n * a_0;
        }


    }

}







void SCI_demo_receive_with_interrupts(void)
{

    uint8_t data;

    while(1)
    {
        // Check if any data in the SCI RX buffer.
        if ( BUF_get_data_size( &SCI_RX_buf_handle) )
        {
            // read the data
            BUF_get_byte( &SCI_RX_buf_handle, &data );

            // and send it back in blocking mode
            SCI_send_byte(data);

        }
    }

}



void SCI_demo_transmit_with_interrupts(void)
{

    char message[100] = "This was sent using USART interrupts!\n";

    while(1)
    {

        SCI_send_string_IT(message);

        HAL_Delay(1000);

    }

}





void SCI_demo_echo_with_interrupts(void)
{

#define N_MAX		500

    float a_0 = 0.5;
    float a_n;
    float sum = 0;





    uint8_t message[SCI_RX_BUF_LEN];
    uint32_t message_size;


    while(1)
    {


        // Check if any data in the SCI RX buffer.
        message_size = BUF_get_data_size( &SCI_RX_buf_handle);
        if ( message_size > 0 )
        {
            // store received bytes in the message
            BUF_get_bytes( &SCI_RX_buf_handle, message, message_size );

            // and transmit the message in the IRQ mode
            SCI_send_bytes_IT(message, message_size);

        }




        // Simulate "microcontroller busy" by calculating the geometric sum.
        // Vary the N_MAX parameter and see the effect on the echo functionality.
        // Also try commenting this section.

        a_n = a_0;
        for(uint32_t n=0; n<N_MAX; n++)
        {
            sum = sum + a_n;
            a_n = a_n * a_0;
        }


    }


}

//returns remaining space in TX buffer
uint32_t SCI_get_tx_buffer_remaining(void){
  return BUF_get_free_size(&SCI_TX_buf_handle);
}

//returns remaining space in RX buffer
uint32_t SCI_get_rx_buffer_remaining(void){
  return BUF_get_free_size(&SCI_RX_buf_handle);
}

//printf style output to SCI serial
void SCI_printf(const char* format, ...){
  uint32_t msg_len_cnt;
  va_list args;
  va_start(args, format);
  msg_len_cnt = 0;
  msg_len_cnt = vsnprintf(SCI_printf_buffer, SCI_PRINTF_BUF_LEN, format, args);
//  while(SCI_printf_buffer[msg_len_cnt] != 0) msg_len_cnt++;
  SCI_send_bytes_IT( (uint8_t*)SCI_printf_buffer, msg_len_cnt);
}









// -------------- Private function implementations -------------



// ----------- printf() customization ----------
//#warning "Trenutno je printf na serijski port izključen"

// Implement a customized _write() function for the printf() customization.
int _write(int file, char *ptr, int len)
{
	// Use SCI_send_bytes() function to implement the _write() function.
	// Explicit type-casting is needed for the input arguments.

	SCI_send_bytes_IT( (uint8_t*)ptr, (uint32_t) len);

	// The _write() function is required to return the length
	// of characters that were sent. We simplify and assume that
	// all characters were successfully sent.
	return len;
}
