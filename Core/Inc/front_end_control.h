//
// Created by Matej Planinšek on 11/07/2023.
//
// FRONT END CONTROL MODULE (fec)

#ifndef FRONT_END_CONTROL_H

#include "stm32g4xx_hal.h"
#include "gpio.h"
#include "tim.h"
#include "main.h"


#define FEC_NUM_CHANNELS 6

enum timEnum{
    timer1,
    timer4
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

void fec_init(void);
void fec_set_shunt_1x(uint8_t channel);
void fec_set_shunt_10x(uint8_t channel);
void fec_set_shunt_100x(uint8_t channel);
void fec_set_shunt_1000x(uint8_t channel);
void fec_enable_current(uint8_t channel);
void fec_disable_current(uint8_t channel);
void fec_set_force_voltage(uint8_t channel, float voltage);



#define FRONT_END_CONTROL_H

#endif //FRONT_END_CONTROL_H
