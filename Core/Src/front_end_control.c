//
// Created by Matej Planinšek on 11/07/2023.
//

#include "front_end_control.h"



/********* variable initialization *********/
const fec_channel_param_t fec_ch_params[FEC_NUM_CHANNELS] = {
        {
                .cur_en_gpio_port = CH1_CUR_EN_GPIO_Port,
                .cur_en_gpio_pin = CH1_CUR_EN_Pin,
                .shnt_100_gpio_port = CH1_SHNTRNG_100_GPIO_Port,
                .shnt_100_gpio_pin = CH1_SHNTRNG_100_Pin,
                .shnt_10_gpio_port = CH1_SHNTRNG_10_GPIO_Port,
                .shnt_10_gpio_pin = CH1_SHNTRNG_10_Pin,
                .shnt_1_gpio_port = CH1_SHNTRNG_1_GPIO_Port,
                .shnt_1_gpio_pin = CH1_SHNTRNG_1_Pin,
                .shnt_1000_resistance = 22000.0f,
                .shnt_100_resistance = 2000.0f,
                .shnt_10_resistance = 198.2f,
                .shnt_1_resistance = 19.8f,
                .pwm_timer = timer4,
                .pwm_tim_channel = TIM_CHANNEL_3,
        },
        {
                .cur_en_gpio_port = CH2_CUR_EN_GPIO_Port,
                .cur_en_gpio_pin = CH2_CUR_EN_Pin,
                .shnt_100_gpio_port = CH2_SHNTRNG_100_GPIO_Port,
                .shnt_100_gpio_pin = CH2_SHNTRNG_100_Pin,
                .shnt_10_gpio_port = CH2_SHNTRNG_10_GPIO_Port,
                .shnt_10_gpio_pin = CH2_SHNTRNG_10_Pin,
                .shnt_1_gpio_port = CH2_SHNTRNG_1_GPIO_Port,
                .shnt_1_gpio_pin = CH2_SHNTRNG_1_Pin,
                .shnt_1000_resistance = 22000.0f,
                .shnt_100_resistance = 2000.0f,
                .shnt_10_resistance = 198.2f,
                .shnt_1_resistance = 19.8f,
                .pwm_timer = timer1,
                .pwm_tim_channel = TIM_CHANNEL_3,
        },
        {
                .cur_en_gpio_port = CH3_CUR_EN_GPIO_Port,
                .cur_en_gpio_pin = CH3_CUR_EN_Pin,
                .shnt_100_gpio_port = CH3_SHNTRNG_100_GPIO_Port,
                .shnt_100_gpio_pin = CH3_SHNTRNG_100_Pin,
                .shnt_10_gpio_port = CH3_SHNTRNG_10_GPIO_Port,
                .shnt_10_gpio_pin = CH3_SHNTRNG_10_Pin,
                .shnt_1_gpio_port = CH3_SHNTRNG_1_GPIO_Port,
                .shnt_1_gpio_pin = CH3_SHNTRNG_1_Pin,
                .shnt_1000_resistance = 22000.0f,
                .shnt_100_resistance = 2000.0f,
                .shnt_10_resistance = 198.2f,
                .shnt_1_resistance = 19.8f,
                .pwm_timer = timer4,
                .pwm_tim_channel = TIM_CHANNEL_4,
        },
        {
                .cur_en_gpio_port = CH4_CUR_EN_GPIO_Port,
                .cur_en_gpio_pin = CH4_CUR_EN_Pin,
                .shnt_100_gpio_port = CH4_SHNTRNG_100_GPIO_Port,
                .shnt_100_gpio_pin = CH4_SHNTRNG_100_Pin,
                .shnt_10_gpio_port = CH4_SHNTRNG_10_GPIO_Port,
                .shnt_10_gpio_pin = CH4_SHNTRNG_10_Pin,
                .shnt_1_gpio_port = CH4_SHNTRNG_1_GPIO_Port,
                .shnt_1_gpio_pin = CH4_SHNTRNG_1_Pin,
                .shnt_1000_resistance = 22000.0f,
                .shnt_100_resistance = 2000.0f,
                .shnt_10_resistance = 198.2f,
                .shnt_1_resistance = 19.8f,
                .pwm_timer = timer4,
                .pwm_tim_channel = TIM_CHANNEL_1,
        },
        {
                .cur_en_gpio_port = CH5_CUR_EN_GPIO_Port,
                .cur_en_gpio_pin = CH5_CUR_EN_Pin,
                .shnt_100_gpio_port = CH5_SHNTRNG_100_GPIO_Port,
                .shnt_100_gpio_pin = CH5_SHNTRNG_100_Pin,
                .shnt_10_gpio_port = CH5_SHNTRNG_10_GPIO_Port,
                .shnt_10_gpio_pin = CH5_SHNTRNG_10_Pin,
                .shnt_1_gpio_port = CH5_SHNTRNG_1_GPIO_Port,
                .shnt_1_gpio_pin = CH5_SHNTRNG_1_Pin,
                .shnt_1000_resistance = 22000.0f,
                .shnt_100_resistance = 2000.0f,
                .shnt_10_resistance = 198.2f,
                .shnt_1_resistance = 19.8f,
                .pwm_timer = timer1,
                .pwm_tim_channel = TIM_CHANNEL_2,
        },
        {
                .cur_en_gpio_port = CH6_CUR_EN_GPIO_Port,
                .cur_en_gpio_pin = CH6_CUR_EN_Pin,
                .shnt_100_gpio_port = CH6_SHNTRNG_100_GPIO_Port,
                .shnt_100_gpio_pin = CH6_SHNTRNG_100_Pin,
                .shnt_10_gpio_port = CH6_SHNTRNG_10_GPIO_Port,
                .shnt_10_gpio_pin = CH6_SHNTRNG_10_Pin,
                .shnt_1_gpio_port = CH6_SHNTRNG_1_GPIO_Port,
                .shnt_1_gpio_pin = CH6_SHNTRNG_1_Pin,
                .shnt_1000_resistance = 22000.0f,
                .shnt_100_resistance = 2000.0f,
                .shnt_10_resistance = 198.2f,
                .shnt_1_resistance = 19.8f,
                .pwm_timer = timer4,
                .pwm_tim_channel = TIM_CHANNEL_2,
        },
};




/**
 * @brief sets shunt range to 1x (all shunt resistors parallel)
 * @param channel Channel
 */
void fec_set_shunt_1x(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  if(channel >= FEC_NUM_CHANNELS || channel < 1){
    //todo: report channel number error
    return;
  }
  //1000x is hardwired on
  //100x on
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_100_gpio_port,
                    fec_ch_params[param_idx].shnt_100_gpio_pin,
                    GPIO_PIN_SET);
  //10x on
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_10_gpio_port,
                    fec_ch_params[param_idx].shnt_10_gpio_pin,
                    GPIO_PIN_SET);
  //1x on
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_1_gpio_port,
                    fec_ch_params[param_idx].shnt_1_gpio_pin,
                    GPIO_PIN_SET);
}

/**
 * @brief sets shunt range to 10x (1000x, 100x, 10x shunts in parallel)
 * @param channel Channel
 */
void fec_set_shunt_10x(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  if(channel >= FEC_NUM_CHANNELS || channel < 1){
    //todo: report channel number error
    return;
  }
  //1000x is hardwired on
  //100x on
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_100_gpio_port,
                    fec_ch_params[param_idx].shnt_100_gpio_pin,
                    GPIO_PIN_SET);
  //10x on
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_10_gpio_port,
                    fec_ch_params[param_idx].shnt_10_gpio_pin,
                    GPIO_PIN_SET);
  //1x off
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_1_gpio_port,
                    fec_ch_params[param_idx].shnt_1_gpio_pin,
                    GPIO_PIN_RESET);
}

/**
 * @brief sets shunt range to 100x (1000x, 100x shunts in parallel)
 * @param channel Channel
 */
void fec_set_shunt_100x(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  if(channel >= FEC_NUM_CHANNELS || channel < 1){
    //todo: report channel number error
    return;
  }
  //1000x is hardwired on
  //100x on
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_100_gpio_port,
                    fec_ch_params[param_idx].shnt_100_gpio_pin,
                    GPIO_PIN_SET);
  //10x off
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_10_gpio_port,
                    fec_ch_params[param_idx].shnt_10_gpio_pin,
                    GPIO_PIN_RESET);
  //1x off
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_1_gpio_port,
                    fec_ch_params[param_idx].shnt_1_gpio_pin,
                    GPIO_PIN_RESET);
}

/**
 * @brief sets shunt range to 1000x (1000x shunt only)
 * @param channel Channel
 */
void fec_set_shunt_1000x(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  if(channel >= FEC_NUM_CHANNELS || channel < 1){
    //todo: report channel number error
    return;
  }
  //1000x is hardwired on
  //100x off
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_100_gpio_port,
                    fec_ch_params[param_idx].shnt_100_gpio_pin,
                    GPIO_PIN_RESET);
  //10x off
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_10_gpio_port,
                    fec_ch_params[param_idx].shnt_10_gpio_pin,
                    GPIO_PIN_RESET);
  //1x off
  HAL_GPIO_WritePin(fec_ch_params[param_idx].shnt_1_gpio_port,
                    fec_ch_params[param_idx].shnt_1_gpio_pin,
                    GPIO_PIN_RESET);
}

