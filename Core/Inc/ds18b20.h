//
// Created by Matej Planinšek on 14/08/2023.
//

// driver for DS18B20 temperature sensor on LED heatsink

#ifndef LIGHTSOAKFW_STM_DS18B20_H
#define LIGHTSOAKFW_STM_DS18B20_H

#include "stm32g4xx_hal.h"
#include "micro_sec.h"

#define DS18B20_PORT   ONEWIRE_GPIO_Port
#define DS18B20_PIN    ONEWIRE_Pin
#define DS18B20_SKIP_ROM   0xCC
#define DS18B20_CONVERT_T  0x44
#define DS18B20_READ_SP   0xBE

//extern float g_tempsns_temp;

float ds18b20_get_temp(void);

void ds18b20_init(void);
uint8_t ds18b20_reset(void);
void ds18b20_write_byte(uint8_t data);
uint8_t ds18b20_read_byte(void);
float ds18b20_read_temperature(void);
void ds18b20_start_conversion(void);
uint8_t ds18b20_try_read_temperature(float *temperature);
uint8_t ds18b20_is_conversion_done(void);
void ds18b20_handler(void);

//helper functions
void ds18b20_set_pin_input(GPIO_TypeDef *GPIOx, uint16_t pin);
void ds18b20_set_pin_output(GPIO_TypeDef *GPIOx, uint16_t pin);



#endif //LIGHTSOAKFW_STM_DS18B20_H


