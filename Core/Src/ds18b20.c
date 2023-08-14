//
// Created by Matej Planinšek on 14/08/2023.
//

#include "ds18b20.h"

float g_tempsns_temp = 0.0f;  // Global temperature variable

/**
 * @brief Sets the specified pin as input for DS18B20.
 * @param GPIOx GPIO port.
 * @param pin Pin number.
 */
void ds18b20_set_pin_input(GPIO_TypeDef *GPIOx, uint16_t pin){
  LL_GPIO_SetPinMode(GPIOx, pin, LL_GPIO_MODE_INPUT);
}

/**
 * @brief Sets the specified pin as output for DS18B20.
 * @param GPIOx GPIO port.
 * @param pin Pin number.
 */
void ds18b20_set_pin_output(GPIO_TypeDef *GPIOx, uint16_t pin){
  LL_GPIO_SetPinMode(GPIOx, pin, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(GPIOx, pin, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_ResetOutputPin(GPIOx, pin);
}

/**
 * @brief Initializes DS18B20, setting its pin as input by default.
 */
void ds18b20_init(void){
  ds18b20_set_pin_input(DS18B20_PORT, DS18B20_PIN);
}

/**
 * @brief Resets the DS18B20 sensor.
 * @return 1 if device is present, 0 otherwise.
 */
uint8_t ds18b20_reset(void){
  ds18b20_set_pin_output(DS18B20_PORT, DS18B20_PIN);
  HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET); // Pull the line low
  usec_delay(480);  // Wait for 480µs
  ds18b20_set_pin_input(DS18B20_PORT, DS18B20_PIN);  // Release the line
  usec_delay(80);   // Wait 15-60µs to check the presence pulse

  if (!HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN))  // DS18B20 pulled the line low
  {
    usec_delay(400);  // Wait for the end of the presence pulse
    return 1;  // Device is present
  }
  return 0;  // No device
}

/**
 * @brief Writes a byte to DS18B20.
 * @param data Byte data to be written.
 */
void ds18b20_write_byte(uint8_t data){
  for (int i = 0; i < 8; i++)
  {
    ds18b20_set_pin_output(DS18B20_PORT, DS18B20_PIN);
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET); // Start bit
    usec_delay(2);  // Wait a little bit
    if (data & (1 << i))  // Write 1
    {
      HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET); // Release the line
      usec_delay(60);
    }
    else  // Write 0
    {
      usec_delay(60);
      HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET); // Release the line
    }
    usec_delay(10);  // Recovery time
  }
}

/**
 * @brief Reads a byte from DS18B20.
 * @return Data byte read from DS18B20.
 */
uint8_t ds18b20_read_byte(void){
  uint8_t data = 0;
  for (int i = 0; i < 8; i++)
  {
    ds18b20_set_pin_output(DS18B20_PORT, DS18B20_PIN);
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET); // Start bit
    usec_delay(2);  // Wait a little bit
    ds18b20_set_pin_input(DS18B20_PORT, DS18B20_PIN);  // Release the line
    usec_delay(10);
    if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN))  // Read the data bit
      data |= (1 << i);
    usec_delay(50);  // Complete the time slot
  }
  return data;
}

/**
 * @brief Initiates temperature conversion on DS18B20.
 */
void ds18b20_start_conversion(void){
  if (!ds18b20_reset())
    return;  // Error: No device detected
  ds18b20_write_byte(DS18B20_SKIP_ROM);
  ds18b20_write_byte(DS18B20_CONVERT_T);
}

/**
 * @brief Attempts to read temperature value from DS18B20.
 * @param temperature Pointer to store the read temperature value.
 * @return 1 if temperature read successfully, 0 otherwise.
 */
uint8_t ds18b20_try_read_temperature(float *temperature){
  if (!ds18b20_reset())
    return 0;  // Error: No device detected
  ds18b20_write_byte(DS18B20_SKIP_ROM);
  ds18b20_write_byte(DS18B20_READ_SP);
  int16_t lsb = ds18b20_read_byte();
  int16_t msb = ds18b20_read_byte();
  // Assuming default 12-bit resolution
  int16_t raw = (msb << 8) | lsb;
  *temperature = raw * 0.0625f;
  return 1;  // Temperature read successfully
}

/**
 * @brief Checks if DS18B20 has completed the temperature conversion.
 * @return 1 if conversion is done, 0 otherwise.
 */
uint8_t ds18b20_is_conversion_done(void){
  ds18b20_set_pin_output(DS18B20_PORT, DS18B20_PIN);
  HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // Pull the line low
  usec_delay(2);  // Short delay
  ds18b20_set_pin_input(DS18B20_PORT, DS18B20_PIN);  // Release the line
  usec_delay(10);
  return HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);  // DS18B20 will keep the line low if not done
}

/**
 * @brief Handler function for DS18B20, checks if conversion is done and updates temperature value.
 * Call periodically.
 */
void ds18b20_handler(void){
  // Check if the conversion is done
  if (ds18b20_is_conversion_done())
  {
    // Read the temperature and update the global variable
    if (ds18b20_try_read_temperature(&g_tempsns_temp))
    {
      // Start the next conversion
      ds18b20_start_conversion();
    }
  }
  // If the conversion is not done, we just return and do nothing
}

/**
 * @brief Returns the most recent temperature reading from DS18B20.
 * @return Temperature value.
 */
float ds18b20_get_temp(void){
  return g_tempsns_temp;
}
