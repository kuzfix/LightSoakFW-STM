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


uint8_t prv_fec_shunt_state[FEC_NUM_CHANNELS];



/********* function implementation *********/

/**
 * @brief initialize front end control.
 * Can be used to reset frontend to stock config (high Z on cell)
 * - shunts on 1000x
 * - current disabled
 * - pwm to 0V
 */
void fec_init(void){

  //disable current on all channels
  fec_disable_current(1);
  fec_disable_current(2);
  fec_disable_current(3);
  fec_disable_current(4);
  fec_disable_current(5);
  fec_disable_current(6);

  //set shunt resistors to 1000x for all channels
  fec_set_shunt_1000x(1);
  fec_set_shunt_1000x(2);
  fec_set_shunt_1000x(3);
  fec_set_shunt_1000x(4);
  fec_set_shunt_1000x(5);
  fec_set_shunt_1000x(6);


  //set pulse values to 0 for all channels
  __HAL_TIM_SET_COMPARE(prv_get_pwm_timer_handle(fec_ch_params[0].pwm_timer), fec_ch_params[0].pwm_tim_channel, 0);
  __HAL_TIM_SET_COMPARE(prv_get_pwm_timer_handle(fec_ch_params[1].pwm_timer), fec_ch_params[1].pwm_tim_channel, 0);
  __HAL_TIM_SET_COMPARE(prv_get_pwm_timer_handle(fec_ch_params[2].pwm_timer), fec_ch_params[2].pwm_tim_channel, 0);
  __HAL_TIM_SET_COMPARE(prv_get_pwm_timer_handle(fec_ch_params[3].pwm_timer), fec_ch_params[3].pwm_tim_channel, 0);
  __HAL_TIM_SET_COMPARE(prv_get_pwm_timer_handle(fec_ch_params[4].pwm_timer), fec_ch_params[4].pwm_tim_channel, 0);
  __HAL_TIM_SET_COMPARE(prv_get_pwm_timer_handle(fec_ch_params[5].pwm_timer), fec_ch_params[5].pwm_tim_channel, 0);
  //start all timers in pwm mode
  HAL_TIM_PWM_Start(prv_get_pwm_timer_handle(fec_ch_params[0].pwm_timer), fec_ch_params[0].pwm_tim_channel);
  HAL_TIM_PWM_Start(prv_get_pwm_timer_handle(fec_ch_params[1].pwm_timer), fec_ch_params[1].pwm_tim_channel);
  HAL_TIM_PWM_Start(prv_get_pwm_timer_handle(fec_ch_params[2].pwm_timer), fec_ch_params[2].pwm_tim_channel);
  HAL_TIM_PWM_Start(prv_get_pwm_timer_handle(fec_ch_params[3].pwm_timer), fec_ch_params[3].pwm_tim_channel);
  HAL_TIM_PWM_Start(prv_get_pwm_timer_handle(fec_ch_params[4].pwm_timer), fec_ch_params[4].pwm_tim_channel);
  HAL_TIM_PWM_Start(prv_get_pwm_timer_handle(fec_ch_params[5].pwm_timer), fec_ch_params[5].pwm_tim_channel);
}

/**
 * @brief enable current for channel
 * @param channel Channel
 */
void fec_enable_current(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);

  //set enable pin high
  HAL_GPIO_WritePin(fec_ch_params[param_idx].cur_en_gpio_port,
                    fec_ch_params[param_idx].cur_en_gpio_pin,
                    GPIO_PIN_SET);
}

/**
 * @brief disable current for channel
 * @param channel Channel
 */
void fec_disable_current(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);

  //set enable pin low
  HAL_GPIO_WritePin(fec_ch_params[param_idx].cur_en_gpio_port,
                    fec_ch_params[param_idx].cur_en_gpio_pin,
                    GPIO_PIN_RESET);
}

/**
 * @brief sets shunt range to 1x (all shunt resistors parallel)
 * @param channel Channel
 */
void fec_set_shunt_1x(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);

  //save shunt state
  prv_fec_shunt_state[param_idx] = shnt_1X;

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
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);

  //save shunt state
  prv_fec_shunt_state[param_idx] = shnt_10X;

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
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);

  //save shunt state
  prv_fec_shunt_state[param_idx] = shnt_100X;

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
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);

  //save shunt state
  prv_fec_shunt_state[param_idx] = shnt_1000X;

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


/**
 * @brief sets the force voltage for the given channel.
 * use fec_enable_current to connect voltage setopint circuit to cell
 * !! takes into account +1V DUT cell negative terminal offset
 * @param channel Channel
 * @param voltage voltage in V
 */
void fec_set_force_voltage(uint8_t channel, float voltage){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);
  //voltage request check
  if(voltage > FEC_MCU_VOLTAGE || voltage < -FEC_CELL_NEG_OFFSET){
    //voltage request out of range
    dbg(Error, "FEC: force voltage request out of range\n");
    return;
  }

  //DUT cell has negative terminal offset, compensate for this
  voltage += FEC_CELL_NEG_OFFSET;

  __HAL_TIM_SET_COMPARE(prv_get_pwm_timer_handle(fec_ch_params[param_idx].pwm_timer),
                        fec_ch_params[param_idx].pwm_tim_channel,
                        prv_get_pwm_value(voltage));
}





/**
 * @brief returns the timer handle for the given timer.
 * This is needed because timer handles are not considered constant at compile time
 * @param timer timer enum
 */
TIM_HandleTypeDef* prv_get_pwm_timer_handle(enum timEnum timer){
  if(timer == timer1) {
    return &htim1;
  }
  else if(timer == timer4){
    return &htim4;
  }
}



/**
 * @brief calculates the pwm value for the given voltage.
 * Takes filter attenuation into account
 * @param voltage DC component of PWM signal
 */
uint32_t prv_get_pwm_value(float voltage){
  const float supplyVoltage = 3.3f; // MCU supply voltage
  const uint16_t counterPeriod = 4095; // PWM counter period

  if(voltage > FEC_MCU_VOLTAGE) {
    voltage = FEC_MCU_VOLTAGE; // Cap at max voltage
    dbg(Warning, "Requested voltage is too high, capping at MCU_SPLYV\n");
  }
  if(voltage < 0) {
    return 0; // No need to calculate if voltage is 0
  }

  uint32_t pwmValue = (uint32_t)((voltage / FEC_MCU_VOLTAGE) * FEC_TIMER_RELOAD);

  //just in case of rounding errors
  if(pwmValue > FEC_TIMER_RELOAD) {
    pwmValue = FEC_TIMER_RELOAD;
  }

  return pwmValue;
}

/**
 * @brief returns the currently set shunt resistance for the given channel
 * @param channel Channel
 */
float fec_get_shunt_resistance(uint8_t channel){
  uint8_t param_idx = channel - 1;

  //invalid channel number check
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 1);

  switch (prv_fec_shunt_state[param_idx]) {
    case shnt_1X:
      return fec_ch_params[param_idx].shnt_1_resistance;
    break;
    case shnt_10X:
      return fec_ch_params[param_idx].shnt_10_resistance;
    break;
    case shnt_100X:
      return fec_ch_params[param_idx].shnt_100_resistance;
    break;
    case shnt_1000X:
      return fec_ch_params[param_idx].shnt_1000_resistance;
    break;

  }
}