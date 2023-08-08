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
  //register commands
  lwshell_register_cmd("getvolt", cli_cmd_getvolt_fn, "Measures voltage. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("getcurr", cli_cmd_getcurr_fn, "Measures current. -c #ch# to select channel. No param for all channels");
  lwshell_register_cmd("getiv_point", cli_cmd_getiv_point_fn, "Measures IV point. -c #ch# to select channel. -v #volt# to set voltage");
  lwshell_register_cmd("getiv_char", cli_cmd_getiv_char_fn, "Measures IV characteristic. -c #ch# to select channel. -vs #volt# start volt, -ve #volt# end volt, -s #volt# step");
  lwshell_register_cmd("measure_dump", cli_cmd_dump_fn, "Measure and dump buffer. -c #ch# to select channel. No param for all channels. -n #num# to set number of samples. -VOLT/-CURR/-IV to select what to dump");
}


int32_t cli_cmd_getvolt_fn(int32_t argc, char** argv){
  if(argc == 3){
    uint32_t ch = 0;
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
    if(ch > 6 || ch < 0){
      mainser_printf("Wrong channel index!\n");
      return 0;
    }
    meas_get_voltage(ch);
    return 0;
  }
  else if(argc == 1){
    //measure all channels if no argument
    meas_get_voltage(0);
    return 0;
  }

  //if not 0 or 1 arguments (+cmd name), print error
  mainser_printf("Wrong number of arguments. Use -h for help.\n");
  return -1;
}

int32_t cli_cmd_getcurr_fn(int32_t argc, char** argv){
  if(argc == 3){
    uint32_t ch = 0;
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
    if(ch > 6 || ch < 0){
      mainser_printf("Wrong channel index!\n");
      return 0;
    }
    meas_get_current(ch);
    return 0;
  }
  else if(argc == 1){
    //measure all channels if no argument
    meas_get_current(0);
    return 0;
  }

  //if not 0 or 1 arguments (+cmd name), print error
  mainser_printf("Wrong number of arguments. Use -h for help.\n");
  return -1;
}

int32_t cli_cmd_getiv_point_fn(int32_t argc, char** argv){
  if(argc == 5){
    uint32_t ch = 0;
    float volt = 0.0f;
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
    cmdsprt_parse_float("-v", &volt, argc, argv);
    if(ch > 6 || ch < 0){
      mainser_printf("Wrong channel index!\n");
      return 0;
    }
    meas_get_IV_point(ch, volt, 1, 0);
    return 0;
  }

  //if not 0 or 1 arguments (+cmd name), print error
  mainser_printf("Wrong number of arguments. Use -h for help.\n");
  return -1;
}

int32_t cli_cmd_getiv_char_fn(int32_t argc, char** argv){
  if(argc == 9){
    uint32_t ch = 0;
    float start = 0.0f;
    float end = 0.0f;
    float step = 0.0f;
    cmdsprt_parse_uint32("-c", &ch, argc, argv);
    cmdsprt_parse_float("-vs", &start, argc, argv);
    cmdsprt_parse_float("-ve", &end, argc, argv);
    cmdsprt_parse_float("-s", &step, argc, argv);
    if(ch > 6 || ch < 0){
      mainser_printf("Wrong channel index!\n");
      return 0;
    }
    meas_get_iv_characteristic(ch, start, end, step);
    return 0;
  }

  //if not 0 or 1 arguments (+cmd name), print error
  mainser_printf("Wrong number of arguments. Use -h for help.\n");
  return -1;
}

int32_t cli_cmd_dump_fn(int32_t argc, char** argv){
  uint32_t ch = 0;
  uint32_t num_samples = 0;
  uint8_t arg_voltcurriv_idx = 4;

  //parse num samples
  cmdsprt_parse_uint32("-n", &num_samples, argc, argv);

  if(argc == 6 || argc == 4){
    //all arguments or no channel argument
    if(argc == 6) {
      cmdsprt_parse_uint32("-c", &ch, argc, argv);
      if(ch > 6 || ch < 0){
        mainser_printf("Wrong channel index!\n");
        return 0;
      }
      arg_voltcurriv_idx = 5;
    }
    else{
      arg_voltcurriv_idx = 3;
    }
    //parse -VOLT/-CURR/-IV
    if(strcmp(argv[arg_voltcurriv_idx], "-VOLT") == 0){
      meas_volt_sample_and_dump(ch, num_samples);
      return 0;
    }
    else if(strcmp(argv[arg_voltcurriv_idx], "-CURR") == 0){
      meas_curr_sample_and_dump(ch, num_samples);
      return 0;
    }
    else if(strcmp(argv[arg_voltcurriv_idx], "-IV") == 0){
      meas_iv_sample_and_dump(ch, num_samples);
      return 0;
    }
    else{
      mainser_printf("Wrong argument. Use -h for help.\n");
      return 0;
    }
  }


  mainser_printf("Wrong number of arguments. Use -h for help.\n");
  return -1;
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