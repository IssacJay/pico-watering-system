/***********************************************************************************
** @file         hw.h
** @brief        Hardware header file for the Pico Irrigation System.
** @author       Issac Thomas
** @date         01 May 2026
** Project       Pico Irrigation System
************************************************************************************
** Revisions:
** V1.0 - 01 May 2026 - Initial version 
************************************************************************************
** This program is the confidential and proprietary product of: Issac Thomas.
** Unauthorised use, reproduction or transfer of this program is strictly prohibited
************************************************************************************
** Copyright 2026 Issac Thomas.
** (Subject to limited distribution and restricted disclosure only)
** All rights reserved.
************************************************************************************/

#ifndef HW_H_
#define HW_H_

/***********************************************************************************
** Defines
*/

#define HW_GPIO_PIN_LED                 25U

typedef enum {
    HW_GPIO_PIN_RELAY_TOP       = 0U,
    HW_GPIO_PIN_RELAY_BOTTOM    = 1U,
    HW_GPIO_PIN_RELAY_NUM_PINS
} HW_gpio_water_pump_pins_t;

typedef enum {
    HW_GPIO_PIN_TEMP_SENSOR_INTERNAL = 2U,
    HW_GPIO_PIN_TEMP_SENSOR_EXTERNAL = 3U,
    HW_GPIO_PIN_TEMP_SENSOR_NUM_PINS
} HW_gpio_temperature_DHT11_pins_t;

#define GPIO_RELAY_ACTIVE 0U
#define GPIO_RELAY_INACTIVE 1U

/***********************************************************************************
** Functions
*/

/***********************************************************************************
** @brief Initialise the hardware
************************************************************************************/
void HW_init(void);


#endif /* HW_H_ */
/***********************************************************************************
** End Of File
*/