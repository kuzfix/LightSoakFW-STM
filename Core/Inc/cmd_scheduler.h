//
// Created by Matej Planinšek on 14/08/2023.
//

#ifndef LIGHTSOAKFW_STM_CMD_SCHEDULER_H
#define LIGHTSOAKFW_STM_CMD_SCHEDULER_H

#include "cmd_line_support.h"
#include "measurements.h"

//todo: determine size
#define CMDSCHED_PARAM_BUFF_LEN 32
#define CMDSCHED_QUEUE_SIZE 10
//cmds are poped from the queue and parsed some time before execution.
//When this happens, code waits in blocking until the time of execution.
#define CMDSCHED_POP_BEFORE_EXEC_US 1000

//todo: implement a check so that cmds are scheduled after now and cronologically one after the other

typedef struct {
    meas_funct_id cmd_id;
    uint64_t exec_time;
    uint8_t params_buff[CMDSCHED_PARAM_BUFF_LEN];
} cmd_sched_t;


//scheduled cmd queue functions
uint8_t cmdsched_q_count();
cmd_sched_t cmdsched_q_pop();
void cmdsched_q_push(cmd_sched_t item);
uint8_t cmdsched_q_free_spaces();
// ------

int8_t cmdsched_encode_and_add(uint64_t exec_time, meas_funct_id cmd_id, void *params, uint8_t params_len);

void cmdsched_decode(cmd_sched_t cmd, void *params, uint8_t params_len);

void cmdsched_handler(void);

#endif //LIGHTSOAKFW_STM_CMD_SCHEDULER_H