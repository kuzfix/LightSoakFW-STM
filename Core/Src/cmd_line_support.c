//
// Created by Matej Planinšek on 08/08/2023.
//
#include "cmd_line_support.h"
#include "lwshell/lwshell.h"

//this is the callback function for the lwshell when it wants to output stuff to cli
void cmdsprt_lwshell_out_callback(const char* str, struct lwshell* lwobj){
  mainser_send_string(str);
}

//sets up cli interface. inits lwshell and registers commands
void cmdsprt_setup_cli(void){
  //welcome message
  mainser_printf("LightSoak CLI started.\r\n");
  mainser_printf("Firmware version: %s\r\n", FW_VERSION);
  mainser_printf("FW compiled: %s %s\r\n", __DATE__, __TIME__);
  mainser_printf("-----------------------------\r\n");
  mainser_printf("Type help to list all commands.\r\n");
  mainser_printf("Type <cmd> -h to get help for a specific command.\r\n");
  mainser_printf("Add -sched ### argument to schedule command at specific time\r\n");
  mainser_printf("See https://github.com/mrmp17/LightSoakFW-STM for more info.\r\n");
  mainser_printf("See https://github.com/mrmp17/LightSoakFW-Python for data logging python interface.\r\n");
  mainser_printf("-----------------------------\r\n");
  mainser_printf("READY\r\n");
  mainser_printf(LWSHELL_PROMPT);
  //init lwshell
  lwshell_init();
  //set lwshel output callback
  lwshell_set_output_fn(cmdsprt_lwshell_out_callback);

  //register commands
  lwshell_register_cmd("getvolt", cli_cmd_getvolt_fn, "Measures voltage. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("getcurr", cli_cmd_getcurr_fn, "Measures current. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("getivpoint", cli_cmd_getiv_point_fn, "Measures IV point. -c #ch# to select channel. -v #volt# to set voltage");
  lwshell_register_cmd("getivchar", cli_cmd_getiv_char_fn, "Measures IV characteristic. -c #ch# to select channel. -vs #volt# start volt, -ve #volt# end volt, -s #volt# step");
  lwshell_register_cmd("measuredump", cli_cmd_dump_fn, "Measure and dump buffer. -c #ch# to select channel. No param for all channels. -n #num# to set number of samples. -VOLT/-CURR/-IV to select what to dump");
  lwshell_register_cmd("setledcurr", cli_cmd_setledcurr_fn, "Set LED current. -i #current[A]# to set current. Temperature compensated");
  lwshell_register_cmd("setledillum", cli_cmd_setledillum_fn, "Set LED illumination. -illum #illumination[sun]# to set led. Temperature compensated");
  lwshell_register_cmd("blinkled", cli_cmd_blinkled_fn, "Blink LED. -i #current[A]# to set current. -t #time[us]# to set time. -n to set number of blinks. No scheduling.");
  lwshell_register_cmd("resettimestamp", cli_cmd_reset_timestamp_fn, "Reset internal 64bit microseconds timer to 0. No scheduling.");
  lwshell_register_cmd("gettimestamp", cli_cmd_get_timestamp_fn, "Get internal 64bit microseconds timer value. No scheduling.");
  lwshell_register_cmd("flashmeasure", cli_cmd_flash_measure_fn, "Flash voltage measurement. -c #ch# to select channel. -illum #illum[sun]# to set illumination. -t #time[us]# to set flash duration. <<-m #time[us]# to set measurement time. -n #num# to set number of averages>> or <<-DUMP to dump buffer>>.");
  lwshell_register_cmd("enablecurrent", cli_cmd_enable_current_fn, "Enable current. -c #ch# to select channel. No param for all channels.");
  lwshell_register_cmd("disablecurrent", cli_cmd_disable_current_fn, "Disable current. -c #ch# to select channel. No param for all channels.");
  lwshell_register_cmd("setshunt", cli_cmd_set_shunt_fn, "Set current shunt range. -c #ch# to select channel. No param for all channels. -1x/-10x/-100x/-100x to set range.");
  lwshell_register_cmd("setforcevolt", cli_cmd_setforcevolt_fn, "Set force voltage. -c #ch# to select channel. No param for all channels. -v #volt# to set voltage.");
  lwshell_register_cmd("autorange", cli_cmd_autorange_fn, "Autorange current shunts on all channels. No scheduling.");
  lwshell_register_cmd("reboot", cli_cmd_reboot_fn, "Reboot the device. No scheduling.");
  lwshell_register_cmd("getledtemp", cli_cmd_getledtemp_fn, "Get LED temperature.");
  lwshell_register_cmd("calibillum", cli_cmd_calib_illum_fn, "Callibrate illumination-current coefficient for LED. Specify a calibrated point with -i #current[A]# -illum #illum[sun]#");
  lwshell_register_cmd("yeet", cli_cmd_yeet_fn, "Y E E E E E T");
  lwshell_register_cmd("setbaud", cli_cmd_setbaud_fn, "sets baud rate. -b #baud# to set baud rate. No scheduling.");
  lwshell_register_cmd("ready?", cli_cmd_ready_fn, "Call to check if ready.");
  lwshell_register_cmd("ENDSEQUENCE", cli_cmd_endseq_fn, "End of sequence signal. prints out END_OF_SEQUENCE and reboots after one second.");
  lwshell_register_cmd("getnumavg", cli_cmd_getnumavg_fn, "Get number of samples to average for getvolt/getcurr.");
  lwshell_register_cmd("setnumavg", cli_cmd_setnumavg_fn, "Set number of samples to average for getvolt/getcurr. -n #num# to set number of samples.");
  lwshell_register_cmd("setdutsettle", cli_cmd_setdutsettle_fn, "Set settling time of DUT for measuring IV points and IV characteristics. -t #settle_time[ms]# to set. In ms.");
  lwshell_register_cmd("getdutsettle", cli_cmd_getdutsettle_fn, "Get settling time of DUT for measuring IV points and IV characteristics In ms.");
  lwshell_register_cmd("getnoise", cli_cmd_getnoise_fn, "Measures noise (RMSmV and SNR) on channels. -c #ch# to select channel. No param for all channels. -VOLT or/and -CURR to measure noise on voltage or/and current channels.");
}


int32_t cli_cmd_getvolt_fn(int32_t argc, char** argv){
  uint32_t ch;

  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);

    // schedule command ##########
    meas_get_voltage_param_t param;
    param.channel = ch;
    cmdsched_encode_and_add(sched_time, meas_get_voltage_id, &param, sizeof(meas_get_voltage_param_t));
    // END schedule command ##########

  }
  else{
    //immediate command
    meas_get_voltage(ch);
  }
  return 0;
}

int32_t cli_cmd_getcurr_fn(int32_t argc, char** argv){
  uint32_t ch;

  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    meas_get_current_param_t param;
    param.channel = ch;
    cmdsched_encode_and_add(sched_time, meas_get_current_id, &param, sizeof(meas_get_current_param_t));
    // END schedule command ##########

  }
  else{
    //immediate command
    meas_get_current(ch);
  }
  return 0;
}

int32_t cli_cmd_getiv_point_fn(int32_t argc, char** argv){
  uint32_t ch;
  float cmdvolt;

  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }
  //parse voltage setpoint
  if(cmdsprt_is_arg("-v", argc, argv)){
    //voltage argument present, parse
    cmdsprt_parse_float("-v", &cmdvolt, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    meas_get_IV_point_param_t param;
    param.channel = ch;
    param.voltage = cmdvolt;
    param.disable_current_when_finished = 1;
    param.noident = 0;
    cmdsched_encode_and_add(sched_time, meas_get_IV_point_id, &param, sizeof(meas_get_IV_point_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    meas_get_exact_IV_point(ch, cmdvolt, 1, 0);
  }
  return 0;
}

int32_t cli_cmd_getiv_char_fn(int32_t argc, char** argv){
  uint32_t ch, Dt=10, N=1;
  float start, end, step;
  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //parse v_start
  if(cmdsprt_is_arg("-vs", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-vs", &start, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //parse v_end
  if(cmdsprt_is_arg("-ve", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-ve", &end, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //parse v_step
  if(cmdsprt_is_arg("-s", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-s", &step, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //parse Dt
  if(cmdsprt_is_arg("-st", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-st", &Dt, argc, argv);
    if ((Dt<1) || (Dt>65535) ) {
    	dbg(Warning, "CLI CMD Error: invalid -st value\r\n");
    }
  }

  //parse N
  if(cmdsprt_is_arg("-sn", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-sn", &N, argc, argv);
    if ((N<1) || (N>Dt) ) {
    	dbg(Warning, "CLI CMD Error: invalid -sn value\r\n");
    }
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    meas_get_iv_characteristic_param_t param;
    param.channel = ch;
    param.start_volt = start;
    param.end_volt = end;
    param.step_volt = step;
    param.step_time = Dt;
    param.Npoints_per_step = N;
    cmdsched_encode_and_add(sched_time, meas_get_iv_characteristic_id, &param, sizeof(meas_get_iv_characteristic_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    meas_get_iv_characteristic(ch, start, end, step, Dt, N);
  }
  return 0;
}

int32_t cli_cmd_dump_fn(int32_t argc, char** argv){
  uint32_t ch;
  uint32_t num_samples;

  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }

  //parse num samples
  if(cmdsprt_is_arg("-n", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-n", &num_samples, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //parse -VOLT/-CURR/-IV
  if(cmdsprt_is_arg("-VOLT", argc, argv)){
    //scheduled or immediate
    if(cmdsprt_is_arg("-sched", argc, argv)){
      //scheduled command
      uint64_t sched_time;
      cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
      // schedule command ##########
      meas_sample_and_dump_param_t param;
      param.channel = ch;
      param.num_samples = num_samples;
      cmdsched_encode_and_add(sched_time, meas_volt_sample_and_dump_id, &param, sizeof(meas_sample_and_dump_param_t));
      // END schedule command ##########
    }
    else{
      //immediate command
      meas_volt_sample_and_dump(ch, num_samples);
    }
  }
  else if(cmdsprt_is_arg("-CURR", argc, argv)){
    //scheduled or immediate
    if(cmdsprt_is_arg("-sched", argc, argv)){
      //scheduled command
      uint64_t sched_time;
      cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
      // schedule command ##########
      meas_sample_and_dump_param_t param;
      param.channel = ch;
      param.num_samples = num_samples;
      cmdsched_encode_and_add(sched_time, meas_curr_sample_and_dump_id, &param, sizeof(meas_sample_and_dump_param_t));
      // END schedule command ##########
    }
    else{
      //immediate command
      meas_curr_sample_and_dump(ch, num_samples);
    }
  }
  else if(cmdsprt_is_arg("-IV", argc, argv)){
    //scheduled or immediate
    if(cmdsprt_is_arg("-sched", argc, argv)){
      //scheduled command
      uint64_t sched_time;
      cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
      // schedule command ##########
      meas_sample_and_dump_param_t param;
      param.channel = ch;
      param.num_samples = num_samples;
      cmdsched_encode_and_add(sched_time, meas_iv_sample_and_dump_id, &param, sizeof(meas_sample_and_dump_param_t));
      // END schedule command ##########
    }
    else{
      //immediate command
      meas_iv_sample_and_dump(ch, num_samples);
    }
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }
  return 0;
}

int32_t cli_cmd_setledcurr_fn(int32_t argc, char** argv){
  float current;

  //parse current
  if(cmdsprt_is_arg("-i", argc, argv)){
    cmdsprt_parse_float("-i", &current, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    ledctrl_set_current_param_t param;
    param.current = current;
    cmdsched_encode_and_add(sched_time, ledctrl_set_current_id, &param, sizeof(ledctrl_set_current_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    ledctrl_set_current_tempcomp(current);
  }
  return 0;
}

int32_t cli_cmd_setledillum_fn(int32_t argc, char** argv){
  float illum;

  //parse current
  if(cmdsprt_is_arg("-illum", argc, argv)){
    cmdsprt_parse_float("-illum", &illum, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    ledctrl_set_illum_param_t param;
    param.illum = illum;
    cmdsched_encode_and_add(sched_time, ledctrl_set_illum_id, &param, sizeof(ledctrl_set_illum_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    ledctrl_set_illum(illum);
  }
  return 0;
}


int32_t cli_cmd_blinkled_fn(int32_t argc, char** argv){
  float current;
  uint32_t dur;
  uint32_t numblinks;
  //parse current
  if(cmdsprt_is_arg("-i", argc, argv)){
    cmdsprt_parse_float("-i", &current, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }
  //parse duration
  if(cmdsprt_is_arg("-t", argc, argv)){
    cmdsprt_parse_uint32("-t", &dur, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }
  //parse numblinks
  if(cmdsprt_is_arg("-n", argc, argv)){
    cmdsprt_parse_uint32("-n", &numblinks, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate
  // THIS DOES NOT SUPPORT SCHEDULING
  // THIS CMD IS FOR TESTING ONLY

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    dbg(Error, "CMD does not support scheduling\r\n");
    return -1;
  }

  for(uint32_t i = 0; i < numblinks; i++){
    ledctrl_set_current_tempcomp(current);
    usec_delay(dur);
    ledctrl_set_current_tempcomp(0.0f);
    usec_delay(dur);
  }

  return 0;
}

int32_t cli_cmd_flash_measure_fn(int32_t argc, char** argv){
  float illum;
  uint32_t flash_dur;
  uint32_t measure_at;
  uint32_t numavg;
  uint32_t ch;

  //parse channel
  if(cmdsprt_is_arg("-c", argc, argv)){
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }

  //parse illum
  if(cmdsprt_is_arg("-illum", argc, argv)){
    cmdsprt_parse_float("-illum", &illum, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //parse flash_dur
  if(cmdsprt_is_arg("-t", argc, argv)){
    cmdsprt_parse_uint32("-t", &flash_dur, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //check if dump
  if(cmdsprt_is_arg("-DUMP", argc, argv)){
    //dump measurement
    //scheduled or immediate

    if(cmdsprt_is_arg("-sched", argc, argv)){
      //scheduled command
      uint64_t sched_time;
      cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
      // schedule command ##########
      meas_flashmeasure_dumpbuffer_param_t param;
      param.channel = ch;
      param.illum = illum;
      param.flash_dur_us = flash_dur;
      cmdsched_encode_and_add(sched_time, meas_flashmeasure_dumpbuffer_id, &param, sizeof(meas_flashmeasure_dumpbuffer_param_t));
      // END schedule command ##########
    }
    else{
      //immediate command
      meas_flashmeasure_dumpbuffer(ch, illum, flash_dur);
    }

  }
  else{
    //singleshot measurement
    //parse measure_at
    if(cmdsprt_is_arg("-m", argc, argv)){
      cmdsprt_parse_uint32("-m", &measure_at, argc, argv);
    }
    else{
      dbg(Warning, "CLI CMD Error\r\n");
      return -1;
    }
    //parse numavg
    if(cmdsprt_is_arg("-n", argc, argv)){
      cmdsprt_parse_uint32("-n", &numavg, argc, argv);
    }
    else{
      dbg(Warning, "CLI CMD Error\r\n");
      return -1;
    }

    //scheduled or immediate

    if(cmdsprt_is_arg("-sched", argc, argv)){
      //scheduled command
      uint64_t sched_time;
      cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
      // schedule command ##########
      meas_flashmeasure_singlesample_param_t param;
      param.channel = ch;
      param.illum = illum;
      param.flash_dur_us = flash_dur;
      param.measure_at_us = measure_at;
      param.numavg = numavg;
      cmdsched_encode_and_add(sched_time, meas_flashmeasure_singlesample_id, &param, sizeof(meas_flashmeasure_singlesample_param_t));
      // END schedule command ##########
    }
    else{
      //immediate command
      meas_flashmeasure_singlesample(ch, illum, flash_dur, measure_at, numavg);
    }

  }
  return 0;
}

int32_t cli_cmd_reset_timestamp_fn(int32_t argc, char** argv){
  usec_reset_timestamp();
  return 0;
}

int32_t cli_cmd_get_timestamp_fn(int32_t argc, char** argv){
  uint64_t timestamp = usec_get_timestamp_64();
  mainser_printf("TIMESTAMP:%llu\r\n", timestamp);
  return 0;
}

int32_t cli_cmd_enable_current_fn(int32_t argc, char** argv){
  uint32_t ch;
  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }
  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    fec_enable_disable_current_param_t param;
    param.channel = ch;
    cmdsched_encode_and_add(sched_time, fec_enable_current_id, &param, sizeof(fec_enable_disable_current_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    fec_enable_current(ch);
  }
  return 0;
}

int32_t cli_cmd_disable_current_fn(int32_t argc, char** argv){
  uint32_t ch;
  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }
  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    fec_enable_disable_current_param_t param;
    param.channel = ch;
    cmdsched_encode_and_add(sched_time, fec_disable_current_id, &param, sizeof(fec_enable_disable_current_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    fec_disable_current(ch);
  }
  return 0;
}

int32_t cli_cmd_setforcevolt_fn(int32_t argc, char** argv){
  uint32_t ch;
  float volt;
  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }
  //parse volt
  if(cmdsprt_is_arg("-v", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-v", &volt, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    fec_setforcevolt_param_t param;
    param.channel = ch;
    param.volt = volt;
    cmdsched_encode_and_add(sched_time, setforcevolt_id, &param, sizeof(fec_setforcevolt_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    fec_set_force_voltage(ch, volt);
  }


  return 0;
}

int32_t cli_cmd_autorange_fn(int32_t argc, char** argv){
  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    cmdsched_encode_and_add(sched_time, autorange_id, 0, 0);
    // END schedule command ##########
  }
  else{
    //immediate command
    daq_autorange();
  }
  return 0;
}

int32_t cli_cmd_reboot_fn(int32_t argc, char** argv){
  NVIC_SystemReset();
  return 0;
}

int32_t cli_cmd_yeet_fn(int32_t argc, char** argv){
  for(int i = 0; i < 20; i++){
    mainser_printf("Y E E E E E T\r\n");
    ledctrl_set_current_tempcomp(1.5f);
    usec_delay(40000);
    ledctrl_set_current_tempcomp(0.0f);
    usec_delay(60000);
  }
  return 0;
}

int32_t cli_cmd_getledtemp_fn(int32_t argc, char** argv){
  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    cmdsched_encode_and_add(sched_time, getledtemp_id, 0, 0);
    // END schedule command ##########
  }
  else{
    //immediate command
    ledctrl_print_temperature_mainser();
  }

  return 0;
}


int32_t cli_cmd_setbaud_fn(int32_t argc, char** argv){
  uint32_t baud;
  //parse baud
  if(cmdsprt_is_arg("-b", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-b", &baud, argc, argv);
    mainser_set_baudrate(baud);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }
  return 0;
}

int32_t cli_cmd_ready_fn(int32_t argc, char** argv){
  mainser_printf("READY\r\n");
  return 0;
}

int32_t cli_cmd_set_shunt_fn(int32_t argc, char** argv){
  uint32_t ch;
  uint32_t shunt;
  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }
  //parse shunt
  if(cmdsprt_is_arg("-1x", argc, argv)){
    shunt = 1;
    fec_set_shunt_1x(ch);
  }
  else if(cmdsprt_is_arg("-10x", argc, argv)){
    shunt = 10;
    fec_set_shunt_10x(ch);
  }
  else if(cmdsprt_is_arg("-100x", argc, argv)){
    shunt = 100;
    fec_set_shunt_100x(ch);
  }
  else if(cmdsprt_is_arg("-1000x", argc, argv)){
    shunt = 1000;
    fec_set_shunt_1000x(ch);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // save params
    // schedule command ##########
    fec_setshunt_param_t param;
    param.channel = ch;
    param.shunt = shunt;
    cmdsched_encode_and_add(sched_time, setshunt_id, &param, sizeof(fec_setshunt_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    switch(shunt){
      case 1:
        fec_set_shunt_1x(ch);
        break;
      case 10:
        fec_set_shunt_10x(ch);
        break;
      case 100:
        fec_set_shunt_100x(ch);
        break;
      case 1000:
        fec_set_shunt_1000x(ch);
        break;
      default:
        dbg(Warning, "CLI CMD Error\r\n");
    }
  }
  return 0;
}

int32_t cli_cmd_endseq_fn(int32_t argc, char** argv){
  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    // schedule command ##########
    cmdsched_encode_and_add(sched_time, end_of_sequence_id, 0, 0);
    // END schedule command ##########
  }
  else{
    //immediate command
    meas_end_of_sequence();
    HAL_Delay(1000);
    NVIC_SystemReset();
  }
  return 0;
}

int32_t cli_cmd_calib_illum_fn(int32_t argc, char** argv){
  float illum;
  float curr;
  //parse illum
  if(cmdsprt_is_arg("-illum", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-illum", &illum, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }
  //parse curr
  if(cmdsprt_is_arg("-i", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-i", &curr, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    //save params
    ledctrl_calibillum_param_t param;
    param.illum = illum;
    param.curr = curr;
    // schedule command ##########
    cmdsched_encode_and_add(sched_time, calibillum_id, &param, sizeof(ledctrl_calibillum_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    ledctrl_calibrate_illum_curr(illum, curr);
  }
  return 0;
}

int32_t cli_cmd_setnumavg_fn(int32_t argc, char** argv){
  uint32_t numavg = 0;
  //parse numavg
  if(cmdsprt_is_arg("-n", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-n", &numavg, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    //save params
    meas_set_num_avg_param_t param;
    param.numavg = numavg;
    // schedule command ##########
    cmdsched_encode_and_add(sched_time, meas_set_numavg_id, &param, sizeof(meas_set_num_avg_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    meas_set_num_avg(numavg);
  }
  return 0;

}

int32_t cli_cmd_getnumavg_fn(int32_t argc, char** argv){
  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    //save params

    // schedule command ##########
    cmdsched_encode_and_add(sched_time, meas_get_numavg_id, 0, 0);
    // END schedule command ##########
  }
  else{
    //immediate command
    prv_meas_print_timestamp(usec_get_timestamp_64());
    mainser_printf("NUMAVG:%lu\r\n", meas_get_num_avg());
  }
  return 0;
}

int32_t cli_cmd_setdutsettle_fn(int32_t argc, char** argv){
  uint32_t stltm = 0;
  //parse settling time
  if(cmdsprt_is_arg("-t", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-t", &stltm, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\r\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    //save params
    meas_set_stltm_param_t param;
    param.settle_time = stltm;
    // schedule command ##########
    cmdsched_encode_and_add(sched_time, meas_set_settle_time_id, &param, sizeof(meas_set_stltm_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    meas_set_settling_time(stltm);
  }
  return 0;
}

int32_t cli_cmd_getdutsettle_fn(int32_t argc, char** argv){
  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    //save params

    // schedule command ##########
    cmdsched_encode_and_add(sched_time, meas_get_settle_time_id, 0, 0);
    // END schedule command ##########
  }
  else{
    //immediate command
    prv_meas_print_timestamp(usec_get_timestamp_64());
    mainser_printf("SETTLE_TIME:%lu\r\n", meas_get_settling_time());
  }
  return 0;
}

int32_t cli_cmd_getnoise_fn(int32_t argc, char** argv){
  uint32_t ch;
  uint8_t vflag = 0;
  uint8_t iflag = 0;

  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    ch = 0;
  }

  if(cmdsprt_is_arg("-VOLT", argc, argv)){
    vflag = 1;
  }
  else if(cmdsprt_is_arg("-CURR", argc, argv)){
    iflag = 1;
  }
  else{
    vflag = 1;
    iflag = 1;
  }


  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)) {
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);

    // schedule command ##########
    meas_get_noise_param_t param;
    param.channel = ch;
    param.volt_flag = vflag;
    param.curr_flag = iflag;
    cmdsched_encode_and_add(sched_time, meas_get_noise_id, &param, sizeof(meas_get_noise_param_t));
    // END schedule command ##########
  }
  else{
    //immediate command
    if(vflag) {
      meas_get_noise_volt(ch);
    }
    if(iflag) {
      meas_get_noise_curr(ch);
    }
  }


  return 0;

}









int8_t cmdsprt_parse_float(const char* arg_str, float* float_out, int32_t argc, char** argv) {
  for (int i = 0; i < argc - 1; i++) {  // -1 because we are looking for the next arg after match
    if (strcmp(argv[i], arg_str) == 0) {
      char *endptr;
      float value = strtof(argv[i + 1], &endptr);

      if (endptr == argv[i + 1] || *endptr != '\0') { // Checking if there was any conversion and no extra characters left after conversion
        return -1;
      }

      *float_out = value;
      return 0;
    }
  }
  return -1;
}

int8_t cmdsprt_parse_uint32(const char* arg_str, uint32_t* uint_out, int32_t argc, char** argv) {
  for (int i = 0; i < argc - 1; i++) {  // -1 because we are looking for the next arg after match
    if (strcmp(argv[i], arg_str) == 0) {
      char *endptr;
      unsigned long value = strtoul(argv[i + 1], &endptr, 10); // Using base 10 for decimal conversion

      // Checking if there was any conversion, no extra characters left after conversion, and value is within range
      if (endptr == argv[i + 1] || *endptr != '\0') {
        return -1;
      }

      *uint_out = (uint32_t)value;
      return 0;
    }
  }
  return -1;
}

int8_t cmdsprt_parse_uint64(const char* arg_str, uint64_t* uint_out, int32_t argc, char** argv) {
  for (int i = 0; i < argc - 1; i++) {  // -1 because we are looking for the next arg after match
    if (strcmp(argv[i], arg_str) == 0) {
      char *endptr;
      unsigned long long value = strtoull(argv[i + 1], &endptr, 10); // Using base 10 for decimal conversion

      // Checking if there was any conversion, no extra characters left after conversion, and value is within range
      if (endptr == argv[i + 1] || *endptr != '\0') {
        return -1;
      }

      *uint_out = (uint64_t)value;
      return 0;
    }
  }
  return -1;
}

//checks if the number of arguments is correct
uint8_t cmdsprt_check_argnum( int32_t argnum, int32_t argc){
  return (argc == argnum);
}

//checks if the argument is present in the argument list
uint8_t cmdsprt_is_arg(const char* arg_str, int32_t argc, char** argv) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], arg_str) == 0) {
      return 1;  // Argument found
    }
  }
  return 0;  // Argument not found
}

void cmdsprt_request_new_cmds(void){
  mainser_printf("REQ_SCHED_CMD\r\n");
}
