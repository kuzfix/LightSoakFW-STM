//
// Created by Matej Planinšek on 14/08/2023.
//

#include "cmd_scheduler.h"


// Statically allocated FIFO queue
static cmd_sched_t cmdsched_q_queue[CMDSCHED_QUEUE_SIZE];

// Indices for front and rear of the queue, initialized to 0
static int cmdsched_q_front = 0;
static int cmdsched_q_rear = 0;

//scheduled cmd queue functions ########################################################
void cmdsched_q_push(cmd_sched_t item) {
  if (cmdsched_q_count() == CMDSCHED_QUEUE_SIZE) {
    // Queue is full, you might want to handle this case, e.g., by logging an error or discarding the item
    return;
  }
  cmdsched_q_queue[cmdsched_q_rear] = item;
  cmdsched_q_rear = (cmdsched_q_rear + 1) % CMDSCHED_QUEUE_SIZE;  // wrap around if necessary
}

cmd_sched_t cmdsched_q_pop() {
  if (cmdsched_q_count() == 0) {
    // Queue is empty, returning an empty item
    cmd_sched_t empty_item = {0, 0, {0}};
    return empty_item;
  }
  cmd_sched_t item = cmdsched_q_queue[cmdsched_q_front];
  cmdsched_q_front = (cmdsched_q_front + 1) % CMDSCHED_QUEUE_SIZE;  // wrap around if necessary
  return item;
}

uint8_t cmdsched_q_count() {
  if (cmdsched_q_rear >= cmdsched_q_front) {
    return cmdsched_q_rear - cmdsched_q_front;
  } else {
    return cmdsched_q_rear + (CMDSCHED_QUEUE_SIZE - cmdsched_q_front);
  }
}

uint8_t cmdsched_q_free_spaces() {
  return CMDSCHED_QUEUE_SIZE - cmdsched_q_count();
}

cmd_sched_t cmdsched_q_peek() {
  if (cmdsched_q_count() == 0) {
    // Queue is empty, returning an empty item
    cmd_sched_t empty_item = {0, 0, {0}};
    return empty_item;
  }
  // Return the item at the front of the queue without removing it
  return cmdsched_q_queue[cmdsched_q_front];
}
//######################################################################


int8_t cmdsched_encode_and_add(uint64_t exec_time, meas_funct_id cmd_id, void *params, uint8_t params_len){
  if(cmdsched_q_free_spaces() == 0){
    dbg(Error, "sched queue full\n");
    mainser_printf("SCHED_FAIL\n");
    return -1;
  }
  cmd_sched_t cmd;
  cmd.cmd_id = cmd_id;
  cmd.exec_time = exec_time;
  memcpy(cmd.params_buff, params, params_len);
  cmdsched_q_push(cmd);
  dbg(Debug, "cmd scheduled at %llu\n", exec_time);
  mainser_printf("SCHED_OK\n");
  return 0;
}

void cmdsched_decode(cmd_sched_t cmd, void *params, uint8_t params_len){
  memcpy(params, cmd.params_buff, params_len);
}

//run this as often as possible
void cmdsched_handler(void){
  if(cmdsched_q_count() == 0){
    //nothing in queue
    return;
  }
  cmd_sched_t cmd = cmdsched_q_peek();
  //time to parse and run?
  if(usec_get_timestamp_64() < cmd.exec_time - CMDSCHED_POP_BEFORE_EXEC_US){
    //nothing to do yet
    return;

  }

  //time to pop, parse and execute cmd
  //data already loaded in cmd from peek. Just pop this one out of the q
  cmdsched_q_pop();
  //parse depending on cmd_id
  switch (cmd.cmd_id) {
    case meas_get_voltage_id: {
      meas_get_voltage_param_t param;
      cmdsched_decode(cmd, &param, sizeof(meas_get_voltage_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_get_voltage(param.channel);
      break;
    }
    case meas_get_current_id: {
      // Declare local variables for this case
      int current;
      // Add your code to handle this case
      printf("meas_get_current_id\n");
      break;
    }
    case meas_get_IV_point_id: {
      // Declare local variables for this case
      int iv_point;
      // Add your code to handle this case
      printf("meas_get_IV_point_id\n");
      break;
    }
    case meas_get_iv_characteristic_id: {
      // Declare local variables for this case
      int iv_characteristic;
      // Add your code to handle this case
      printf("meas_get_iv_characteristic_id\n");
      break;
    }
    case meas_sample_and_dump_id: {
      // Declare local variables for this case
      int sample, dump;
      // Add your code to handle this case
      printf("meas_sample_and_dump_id\n");
      break;
    }
    case ledctrl_set_current_id: {
      // Declare local variables for this case
      int led_current;
      // Add your code to handle this case
      printf("ledctrl_set_current_id\n");
      break;
    }
    case meas_flashmeasure_dumpbuffer_id: {
      // Declare local variables for this case
      int buffer;
      // Add your code to handle this case
      printf("meas_flashmeasure_dumpbuffer_id\n");
      break;
    }
    case meas_flashmeasure_singlesample_id: {
      // Declare local variables for this case
      int single_sample;
      // Add your code to handle this case
      printf("meas_flashmeasure_singlesample_id\n");
      break;
    }
    default: {
      // Handle other cases or errors
      printf("Unknown command id\n");
      break;
    }
  }

}