//
// Created by Matej Planinšek on 14/08/2023.
//

#include "cmd_scheduler.h"
#include "UserGPIO.h"

//Scheduler running status flag
static uint8_t sch_running=0;
#define isSchRunning() (sch_running == 1) //safety macro

// time of last scheduled cmd for chronological scheduling check
static uint64_t cmdsched_last_scheduled_time = 0;

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

uint32_t cmdsched_q_count() {
  if (cmdsched_q_rear >= cmdsched_q_front) {
    return cmdsched_q_rear - cmdsched_q_front;
  } else {
    return cmdsched_q_rear + (CMDSCHED_QUEUE_SIZE - cmdsched_q_front);
  }
}

uint32_t cmdsched_q_free_spaces() {
  //todo: is -1 needed?
  if(cmdsched_q_count() < CMDSCHED_QUEUE_SIZE-1){
    return CMDSCHED_QUEUE_SIZE - cmdsched_q_count();
  }
  return 0;
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

void cmdsched_start() {
  if (sch_running == 0)
  {
    usec_reset_timestamp();
    sch_running = 1;
  }
}

//######################################################################


int8_t cmdsched_encode_and_add(uint64_t exec_time, meas_funct_id cmd_id, void *params, uint8_t params_len){
  uint32_t free_space = cmdsched_q_free_spaces();
  if(free_space == 0){
    dbg(Error, "sched queue full\n");
    mainser_printf("SCHED_FAIL\r\n");
    return -1;
  }
  if(exec_time < cmdsched_last_scheduled_time){
    dbg(Error, "sched time before last sched time\n");
    mainser_printf("SCHED_FAIL:NOT_CHRONOLOGICAL\r\n");
    return -1;
  }
  cmdsched_last_scheduled_time = exec_time;
  cmd_sched_t cmd;
  cmd.cmd_id = cmd_id;
  cmd.exec_time = exec_time;
  memcpy(cmd.params_buff, params, params_len);
  cmdsched_q_push(cmd);
  dbg(Debug, "cmd scheduled at %llu\n", exec_time);
  mainser_printf("SCHED_OK\r\n");

  //if schedule queue full or complete sequence loaded, start execution
  free_space = cmdsched_q_free_spaces();
  if (free_space == 0)
    cmdsched_start();
  if (cmd_id == end_of_sequence_id)
    cmdsched_start();

  //request new cmd if time
  //return time left until next cmd is to be executed
  if(cmdsched_q_count() == 0){
    //nothing in queue, we have a lot of time, request sched
    cmdsprt_request_new_cmds();
    return 0;
  }
  cmd = cmdsched_q_peek();

  if (isSchRunning())
  {
    uint64_t tnow = usec_get_timestamp_64();
    //check that if time left is actually positive (scheduled cmds can be late...)
    if((tnow + CMDSCHED_POP_BEFORE_EXEC_US) >  cmd.exec_time){
      //we are already late, no time to transfer cmds. return 0
      return 0;
    }

    uint64_t time_to_cmd =  cmd.exec_time - usec_get_timestamp_64() - CMDSCHED_POP_BEFORE_EXEC_US;
    if(time_to_cmd > MIN_TIME_TO_CMD_TO_REQ_CMDS_US){
      // send request for new cmds to put in cmd queue
      cmdsprt_request_new_cmds();
    }
  }
  else
  {
    cmdsprt_request_new_cmds();
  }

  // ##

  return 0;
}

void cmdsched_decode(cmd_sched_t cmd, void *params, uint8_t params_len){
  memcpy(params, cmd.params_buff, params_len);
}

//run this as often as possible. returns time left until next cmd is to be executed
uint64_t cmdsched_handler(void){
  if(cmdsched_q_count() == 0 || !isSchRunning()){
    //nothing in queue. we have a lot of time.
    return 0xFFFFFFFFFFFFFFFF;
  }
  uint64_t time_now = usec_get_timestamp_64();
  cmd_sched_t cmd = cmdsched_q_peek();
  //time to parse and run?
  if(time_now < cmd.exec_time - CMDSCHED_POP_BEFORE_EXEC_US){
    //nothing to do yet. return time to next cmd
    return cmd.exec_time - time_now - CMDSCHED_POP_BEFORE_EXEC_US;

  }

  //time to pop, parse and execute cmd
  //data already loaded in cmd from peek. Just pop this one out of the q
  cmdsched_q_pop();
  //parse depending on cmd_id
  switch (cmd.cmd_id) {
    case mppt_start_id: {
      mppt_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(mppt_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      mppt_start(param.channel, param.settling_time, param.report_every_xth_point);
      break;
    }
    case mppt_resume_id: {
      mppt_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(mppt_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      mppt_resume(param.channel, param.settling_time, param.report_every_xth_point);
      break;
    }
    case mppt_stop_id: {
      mainser_printf("\r\n");
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      mppt_stop();
      break;
    }
    case meas_get_voltage_id: {
      meas_get_voltage_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_get_voltage_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_get_voltage(param.channel);
      break;
    }
    case meas_get_current_id: {
      meas_get_current_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_get_current_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_get_current(param.channel);
      break;
    }
    case meas_get_IV_point_id: {
      meas_get_IV_point_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_get_IV_point_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_get_exact_IV_point(param.channel, param.voltage, param.disable_current_when_finished, param.noident);
      break;
    }
    case meas_get_iv_characteristic_id: {
      meas_get_iv_characteristic_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_get_iv_characteristic_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_get_iv_characteristic(param.channel, param.start_volt, param.end_volt, param.step_volt, param.step_time, param.Npoints_per_step);
      break;
    }
    case meas_volt_sample_and_dump_id: {
      meas_sample_and_dump_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_sample_and_dump_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_volt_sample_and_dump(param.channel, param.num_samples);
      break;
    }
    case meas_curr_sample_and_dump_id: {
      meas_sample_and_dump_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_sample_and_dump_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_curr_sample_and_dump(param.channel, param.num_samples);
      break;
    }
    case meas_iv_sample_and_dump_id: {
      meas_sample_and_dump_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_sample_and_dump_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_iv_sample_and_dump(param.channel, param.num_samples);
      break;
    }
    case ledctrl_set_current_id: {
      ledctrl_set_current_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(ledctrl_set_current_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      ledctrl_set_current_tempcomp(param.current);
      break;
    }
    case ledctrl_set_illum_id: {
      ledctrl_set_illum_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(ledctrl_set_illum_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      ledctrl_set_illum(param.illum);
      break;
    }
    case meas_flashmeasure_dumpbuffer_id: {
      meas_flashmeasure_dumpbuffer_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_flashmeasure_dumpbuffer_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_flashmeasure_dumpbuffer(param.channel, param.illum, param.flash_dur_us);
      break;
    }
    case meas_flashmeasure_singlesample_id: {
      meas_flashmeasure_singlesample_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_flashmeasure_singlesample_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_flashmeasure_singlesample(param.channel, param.illum, param.flash_dur_us, param.measure_at_us, param.numavg);
      break;
    }
    case end_of_sequence_id: {
      mainser_printf("\r\n");
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_end_of_sequence();
      HAL_Delay(1000);
      NVIC_SystemReset();
      break;
    }
    case fec_enable_current_id: {
      fec_enable_disable_current_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(fec_enable_disable_current_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      fec_enable_current(param.channel);
      break;
    }
    case fec_disable_current_id: {
      fec_enable_disable_current_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(fec_enable_disable_current_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      fec_disable_current(param.channel);
      break;
    }
    case setshunt_id: {
      fec_setshunt_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(fec_setshunt_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);

      switch(param.shunt){
        case 1:
          fec_set_shunt_1x(param.channel);
          break;
        case 10:
          fec_set_shunt_10x(param.channel);
          break;
        case 100:
          fec_set_shunt_100x(param.channel);
          break;
        case 1000:
          fec_set_shunt_1000x(param.channel);
          break;
        default:
          break;
      }
    }
    case setforcevolt_id: {
      fec_setforcevolt_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(fec_setforcevolt_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      fec_set_force_voltage(param.channel, param.volt);
      break;
    }
    case autorange_id: {
      mainser_printf("\r\n");
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      daq_autorange();
      break;
    }
    case getledtemp_id: {
      mainser_printf("\r\n");
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      ledctrl_print_temperature_mainser();
      break;
    }
    case calibillum_id: {
      mainser_printf("\r\n");
      ledctrl_calibillum_param_t param;
      cmdsched_decode(cmd, &param, sizeof(ledctrl_calibillum_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      ledctrl_calibrate_illum_curr(param.illum, param.curr, param.a, param.b, param.c);
      break;
    }
    case meas_set_numavg_id: {
      mainser_printf("\r\n");
      meas_set_num_avg_param_t param;
      cmdsched_decode(cmd, &param, sizeof(meas_set_num_avg_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_set_num_avg(param.numavg);
      break;
    }
    case meas_get_numavg_id: {
      mainser_printf("\r\n");
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      prv_meas_print_timestamp(usec_get_timestamp_64());
      mainser_printf("NUMAVG:%lu\r\n", meas_get_num_avg());
      break;
    }
    case meas_get_settle_time_id: {
      meas_set_stltm_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_set_stltm_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      prv_meas_print_timestamp(usec_get_timestamp_64());
      mainser_printf("SETTLE_TIME:%lu\r\n", meas_get_settling_time());
      break;
    }
    case meas_set_settle_time_id: {
      meas_set_stltm_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_set_stltm_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      meas_set_settling_time(param.settle_time);
      break;
    }
    case meas_get_noise_id: {
      meas_get_noise_param_t param;
      mainser_printf("\r\n");
      cmdsched_decode(cmd, &param, sizeof(meas_get_noise_param_t));
      //wait for exact time to call function
      while(usec_get_timestamp_64() < cmd.exec_time);
      if(param.volt_flag) {
        meas_get_noise_volt(param.channel);
      }
      if(param.curr_flag) {
        meas_get_noise_curr(param.channel);
      }
      break;
    }


    default: {
      // Handle other cases or errors
      printf("Unknown command id\n");
      break;
    }
  }
  //return time left until next cmd is to be executed
  if(cmdsched_q_count() == 0){
    //nothing in queue, we have a lot of time, request sched
    cmdsprt_request_new_cmds();
    return 0xFFFFFFFFFFFFFFFF;
  }
  cmd = cmdsched_q_peek();

  uint64_t tnow = usec_get_timestamp_64();
  //check that if time left is actually positive (scheduled cmds can be late...)
  if((tnow + CMDSCHED_POP_BEFORE_EXEC_US) >  cmd.exec_time){
    //we are already late, no time to transfer cmds. return 0
    return 0;
  }

  uint64_t time_to_cmd =  cmd.exec_time - usec_get_timestamp_64() - CMDSCHED_POP_BEFORE_EXEC_US;
  if(time_to_cmd > MIN_TIME_TO_CMD_TO_REQ_CMDS_US){
    // send request for new cmds to put in cmd queue
    cmdsprt_request_new_cmds();
  }
  return time_to_cmd;
}
