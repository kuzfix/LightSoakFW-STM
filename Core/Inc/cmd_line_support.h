//
// Created by Matej Planinšek on 08/08/2023.
//

/**
 * @brief Includes support functions for the command line interface
 * such as cmd callbacks cmd registrations, etc.
 *
 */

#ifndef LIGHTSOAKFW_STM_CMD_LINE_SUPPORT_H
#define LIGHTSOAKFW_STM_CMD_LINE_SUPPORT_H

#include "stm32g4xx_hal.h"
#include "lwshell/lwshell.h"
#include "main_serial.h"
#include <string.h>
#include "measurements.h"
#include "ds18b20.h"
#include "cmd_scheduler.h"


void cmdsprt_setup_cli(void);

// cmd callback functions (callback for every command)
int32_t cli_cmd_getvolt_fn(int32_t argc, char** argv);
int32_t cli_cmd_getcurr_fn(int32_t argc, char** argv);
int32_t cli_cmd_getiv_point_fn(int32_t argc, char** argv);
int32_t cli_cmd_getiv_char_fn(int32_t argc, char** argv);
int32_t cli_cmd_dump_fn(int32_t argc, char** argv);
int32_t cli_cmd_setledcurr_fn(int32_t argc, char** argv);
int32_t cli_cmd_setledillum_fn(int32_t argc, char** argv);
int32_t cli_cmd_blinkled_fn(int32_t argc, char** argv);


int32_t cli_cmd_reset_timestamp_fn(int32_t argc, char** argv);
int32_t cli_cmd_get_timestamp_fn(int32_t argc, char** argv);
int32_t cli_cmd_flash_measure_fn(int32_t argc, char** argv);

int32_t cli_cmd_enable_current_fn(int32_t argc, char** argv);
int32_t cli_cmd_disable_current_fn(int32_t argc, char** argv);

int32_t cli_cmd_setforcevolt_fn(int32_t argc, char** argv);
int32_t cli_cmd_autorange_fn(int32_t argc, char** argv);

int32_t cli_cmd_getledtemp_fn(int32_t argc, char** argv);

int32_t cli_cmd_yeet_fn(int32_t argc, char** argv);

int32_t cli_cmd_reboot_fn(int32_t argc, char** argv);

int32_t cli_cmd_setbaud_fn(int32_t argc, char** argv);

int32_t cli_cmd_ready_fn(int32_t argc, char** argv);

int32_t cli_cmd_set_shunt_fn(int32_t argc, char** argv);



//todo: implement
int32_t cli_cmd_setnumavg_fn(int32_t argc, char** argv);
int32_t cli_cmd_getnumavg_fn(int32_t argc, char** argv);

//todo:
//reboot


//parser
int8_t cmdsprt_parse_float(const char* arg_str, float* float_out, int32_t argc, char** argv);
int8_t cmdsprt_parse_uint32(const char* arg_str, uint32_t* uint_out, int32_t argc, char** argv);
int8_t cmdsprt_parse_uint64(const char* arg_str, uint64_t* uint_out, int32_t argc, char** argv);
uint8_t cmdsprt_check_argnum( int32_t argnum, int32_t argc);
uint8_t cmdsprt_is_arg(const char* arg_str, int32_t argc, char** argv);


//lwshell out callback
void cmdsprt_lwshell_out_callback(const char* str, struct lwshell* lwobj);

#endif //LIGHTSOAKFW_STM_CMD_LINE_SUPPORT_H
