//
// Created by Matej Planinšek on 11/07/2023.
//
// FRONT END CONTROL MODULE (fec)

#ifndef FRONT_END_CONTROL_H

#include "stm32g4xx_hal.h"
#include "gpio.h"
#include "tim.h"
#include "main.h"
#include "debug.h"


#define FEC_NUM_CHANNELS 6
#define FEC_MCU_VOLTAGE 3.2969f
#define FEC_TIMER_RELOAD 4095
#define FEC_CELL_NEG_OFFSET 1.0f

//todo: check if 4095 or 4096 is corrct for calcs?

enum timEnum{
    timer1,
    timer4
};

enum shntEnum{
    shnt_1X,
    shnt_10X,
    shnt_100X,
    shnt_1000X
};

/**
 * @brief Struct containing channel parameters for front end control
 */
typedef struct{
    GPIO_TypeDef* cur_en_gpio_port;
    uint16_t cur_en_gpio_pin;
    GPIO_TypeDef* shnt_100_gpio_port;
    uint16_t shnt_100_gpio_pin;
    GPIO_TypeDef* shnt_10_gpio_port;
    uint16_t shnt_10_gpio_pin;
    GPIO_TypeDef* shnt_1_gpio_port;
    uint16_t shnt_1_gpio_pin;
    float shnt_1000_resistance;
    float shnt_100_resistance;
    float shnt_10_resistance;
    float shnt_1_resistance;
    enum timEnum pwm_timer;
    uint32_t pwm_tim_channel;

} fec_channel_param_t;


/**
 * @brief Array of channel parameters for front end control
 * @note WARNING:channel indexing stars at 0 !!! To get Channel 1, use index 0
 */
extern const fec_channel_param_t fec_ch_params[FEC_NUM_CHANNELS];




// ##############################  FUNCTION PROTOTYPES  ##############################

TIM_HandleTypeDef* prv_get_pwm_timer_handle(enum timEnum timer);
uint32_t prv_get_pwm_value(float voltage);

void fec_init(void);
void fec_set_shunt_1x(uint8_t channel);
void fec_set_shunt_10x(uint8_t channel);
void fec_set_shunt_100x(uint8_t channel);
void fec_set_shunt_1000x(uint8_t channel);
void fec_enable_current(uint8_t channel);
void fec_disable_current(uint8_t channel);
void fec_set_force_voltage(uint8_t channel, float voltage);
float fec_get_shunt_resistance(uint8_t channel);

// ##############################  END OF FUNCTION PROTOTYPES  ##############################



#define FRONT_END_CONTROL_H

#endif //FRONT_END_CONTROL_H
