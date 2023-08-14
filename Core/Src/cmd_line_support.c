//
// Created by Matej Planinšek on 08/08/2023.
//
#include "cmd_line_support.h"

//this is the callback function for the lwshell when it wants to output stuff to cli
void cmdsprt_lwshell_out_callback(const char* str, struct lwshell* lwobj){
  mainser_send_string(str);
}

//sets up cli interface. inits lwshell and registers commands
void cmdsprt_setup_cli(void){
  //init lwshell
  lwshell_init();
  //set lwshel output callback
  lwshell_set_output_fn(cmdsprt_lwshell_out_callback);
  //welcome message
  mainser_printf("LightSoak CLI started.\n");
  mainser_printf("Type help to list all commands.\n");
  mainser_printf("Type <cmd> -h to get help for a specific command.\n");
  mainser_printf("Add -sched ### argument to schedule command at specific time\n");
  //register commands
  lwshell_register_cmd("getvolt", cli_cmd_getvolt_fn, "Measures voltage. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("getcurr", cli_cmd_getcurr_fn, "Measures current. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("getiv_point", cli_cmd_getiv_point_fn, "Measures IV point. -c #ch# to select channel. -v #volt# to set voltage");
  lwshell_register_cmd("getiv_char", cli_cmd_getiv_char_fn, "Measures IV characteristic. -c #ch# to select channel. -vs #volt# start volt, -ve #volt# end volt, -s #volt# step");
  lwshell_register_cmd("measure_dump", cli_cmd_dump_fn, "Measure and dump buffer. -c #ch# to select channel. No param for all channels. -n #num# to set number of samples. -VOLT/-CURR/-IV to select what to dump");
  lwshell_register_cmd("setledcurr", cli_cmd_setledcurr_fn, "Set LED current. -i #current[A]# to set current.");
  lwshell_register_cmd("blinkled", cli_cmd_blinkled_fn, "Blink LED. -i #current[A]# to set current. -t #time[ms]# to set time.");
  lwshell_register_cmd("reset_timestamp", cli_cmd_reset_timestamp_fn, "Reset internal 64bit microseconds timer to 0.");
  lwshell_register_cmd("get_timestamp", cli_cmd_get_timestamp_fn, "Get internal 64bit microseconds timer value.");
  lwshell_register_cmd("flashmeasure", cli_cmd_flash_measure_fn, "Flash voltage measurement. -c #ch# to select channel. -illum #illum[sun]# to set illumination. -t #time[us]# to set flash duration. <<-m #time[us]# to set measurement time. -n #num# to set number of averages>> or <<-DUMP to dump buffer>>.");
  lwshell_register_cmd("enable_current", cli_cmd_enable_current_fn, "Enable current. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("disable_current", cli_cmd_disable_current_fn, "Disable current. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("setforcevolt", cli_cmd_setforcevolt_fn, "Set force voltage. -c #ch# to select channel. No param for all channels. -v #volt# to set voltage");
  lwshell_register_cmd("autorange", cli_cmd_autorange_fn, "Autorange current shunts on all channels");
  lwshell_register_cmd("reboot", cli_cmd_reboot_fn, "Reboot the device");
  lwshell_register_cmd("yeet", cli_cmd_yeet_fn, "Y E E E E E T");
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
    dbg(Debug, "cmd scheduled for %llu\n", sched_time);
    //todo
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
    dbg(Debug, "cmd scheduled for %llu\n", sched_time);
    //todo
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
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }
  //parse voltage setpoint
  if(cmdsprt_is_arg("-v", argc, argv)){
    //voltage argument present, parse
    cmdsprt_parse_float("-v", &cmdvolt, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    dbg(Debug, "cmd scheduled for %llu\n", sched_time);
    //todo
  }
  else{
    //immediate command
    meas_get_IV_point(ch, cmdvolt, 1, 0);
  }
  return 0;
}

int32_t cli_cmd_getiv_char_fn(int32_t argc, char** argv){
  uint32_t ch;
  float start, end, step;
  //parse ch
  if(cmdsprt_is_arg("-c", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //parse v_start
  if(cmdsprt_is_arg("-vs", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-vs", &start, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //parse v_end
  if(cmdsprt_is_arg("-ve", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-ve", &end, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //parse v_step
  if(cmdsprt_is_arg("-s", argc, argv)){
    //channel argument present, parse
    cmdsprt_parse_float("-s", &step, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }


  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    dbg(Debug, "cmd scheduled for %llu\n", sched_time);
    //todo
  }
  else{
    //immediate command
    meas_get_iv_characteristic(ch, start, end, step);
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
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //parse -VOLT/-CURR/-IV
  if(cmdsprt_is_arg("-VOLT", argc, argv)){
    //scheduled or immediate
    if(cmdsprt_is_arg("-sched", argc, argv)){
      //scheduled command
      uint64_t sched_time;
      cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
      dbg(Debug, "cmd scheduled for %llu\n", sched_time);
      //todo
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
      dbg(Debug, "cmd scheduled for %llu\n", sched_time);
      //todo
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
      dbg(Debug, "cmd scheduled for %llu\n", sched_time);
      //todo
    }
    else{
      //immediate command
      meas_iv_sample_and_dump(ch, num_samples);
    }
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
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
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //scheduled or immediate

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    uint64_t sched_time;
    cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
    dbg(Debug, "cmd scheduled for %llu\n", sched_time);
    //todo
  }
  else{
    //immediate command
    ledctrl_set_current(current);
  }
  return 0;
}


int32_t cli_cmd_blinkled_fn(int32_t argc, char** argv){
  float current;
  uint32_t dur;
  //parse current
  if(cmdsprt_is_arg("-i", argc, argv)){
    cmdsprt_parse_float("-i", &current, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }
//parse duration
  if(cmdsprt_is_arg("-t", argc, argv)){
    cmdsprt_parse_uint32("-t", &dur, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //scheduled or immediate
  // THIS DOES NOT SUPPORT SCHEDULING
  // THIS CMD IS FOR TESTING ONLY

  if(cmdsprt_is_arg("-sched", argc, argv)){
    //scheduled command
    dbg(Error, "CMD does not support scheduling\n");
    return -1;
  }

  ledctrl_set_current(current);
  usec_delay(dur);
  ledctrl_set_current(0.0f);

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
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }

  //parse flash_dur
  if(cmdsprt_is_arg("-t", argc, argv)){
    cmdsprt_parse_uint32("-t", &flash_dur, argc, argv);
  }
  else{
    dbg(Warning, "CLI CMD Error\n");
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
      dbg(Debug, "cmd scheduled for %llu\n", sched_time);
      //todo
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
      dbg(Warning, "CLI CMD Error\n");
      return -1;
    }
    //parse numavg
    if(cmdsprt_is_arg("-n", argc, argv)){
      cmdsprt_parse_uint32("-n", &numavg, argc, argv);
    }
    else{
      dbg(Warning, "CLI CMD Error\n");
      return -1;
    }

    //scheduled or immediate

    if(cmdsprt_is_arg("-sched", argc, argv)){
      //scheduled command
      uint64_t sched_time;
      cmdsprt_parse_uint64("-sched", &sched_time, argc, argv);
      dbg(Debug, "cmd scheduled for %llu\n", sched_time);
      //todo
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
  mainser_printf("TIMESTAMP:%llu\n", timestamp);
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
  fec_enable_current(ch);
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
  fec_disable_current(ch);
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
    dbg(Warning, "CLI CMD Error\n");
    return -1;
  }
  fec_set_force_voltage(ch, volt);
  return 0;
}

int32_t cli_cmd_autorange_fn(int32_t argc, char** argv){
  daq_autorange();
  return 0;
}

int32_t cli_cmd_reboot_fn(int32_t argc, char** argv){
  NVIC_SystemReset();
  return 0;
}

int32_t cli_cmd_yeet_fn(int32_t argc, char** argv){
  for(int i = 0; i < 20; i++){
    mainser_printf("Y E E E E E T\n");
    ledctrl_set_current(1.5f);
    usec_delay(40000);
    ledctrl_set_current(0.0f);
    usec_delay(60000);
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

