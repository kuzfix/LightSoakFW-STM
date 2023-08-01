//
// Created by Matej Planinšek on 12/07/2023.
//
//How to use:
// 0. Enable UART in CubeMX, set the baud rate and enable DMA. Enable global interrupts.
// 1. Copy debug.h and debug.c to your project.
// 2. Include debug.h in your project
// 3. Define the debug level, the UART handle and buffer size in debug.h (## USER PARAMETERS ##)
// 4. Call dbg() function with message level and message (use like printf)
//      example: dbg(Error, "This is an error message. Value: %d\n", 404);
//      level can be: Error, Warning, Debug
//
//
// Note: debug messages are sent out using DMA, so they don't block the CPU. However,
// if dbg() is called again before the previous message is fully transferred, dbg() waits
// (blocking mode) for the previous message to finish transferring before sending the new one.

//
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <string.h>

// Define the debug levels
#define DBG_NONE    0
#define DBG_ERROR   1
#define DBG_WARNING 2
#define DBG_ALL     3

// ############################## USER PARAMETERS ##############################
// Set the debug level here.
#define DEBUG_LEVEL DBG_ALL
//set the UART handle here
#define DEBUG_UART_HANDLE &hlpuart1
//set the buffer size here
#define DEBUG_BUFFER_SIZE 256
// ############################ END USER PARAMETERS ###########################



// Define the debug messages
#define DBG_MSG_ERROR   "E: "
#define DBG_MSG_WARNING "W: "
#define DBG_MSG_DEBUG   "DBG: "

// Debug levels enum
typedef enum {
    Error,
    Warning,
    Debug
} DebugLevel;

void dbg(DebugLevel level, const char* format, ...);

#endif //DEBUG_H