/***********************************************************************************
** @file         drv.c
** @brief        Hardware driver source file for the Pico Irrigation System.
** @author       Issac Thomas
** @date         02 May 2026
** Project       Pico Irrigation System
************************************************************************************
** Revisions:
** V1.0 - 02 May 2026 - Initial version 
************************************************************************************
** This program is the confidential and proprietary product of: Issac Thomas.
** Unauthorised use, reproduction or transfer of this program is strictly prohibited
************************************************************************************
** Copyright 2026 Issac Thomas.
** (Subject to limited distribution and restricted disclosure only)
** All rights reserved.
************************************************************************************/

/***********************************************************************************
 *  Includes
 */ 
#include <stdio.h>
#include "pico/stdlib.h"

#include "drv.h"
#include "../hw/hw.h"
#include "../sys/sys.h"

/***********************************************************************************
** Functions
 */
void DRV_init(void) {

    /* Initialise Relay Drivers */
    for (uint16_t relay = 0; relay < SYS_NUM_WATER_PUMPS; relay++) {
        DRV_water_pump_relays[relay] = (DRV_relay_t){
            .relay_id = (SYS_water_pumps_t)relay,
            .gpio_pin = (HW_gpio_water_pump_pins_t)relay,
            .state = false
        };
        DRV_relay_set_state(DRV_water_pump_relays[relay], false);
    }

    /* Initialise DHT11 Temperature/Humidity Sensors */
    for (uint16_t sensor = 0; sensor < HW_GPIO_PIN_TEMP_SENSOR_NUM_PINS; sensor++) {
        DRV_dht11_sensors[sensor] = (DRV_dht11_sensor_t){
            .sensor_id = sensor,
            .gpio_pin = (HW_gpiotemperature_DH11_pins_t)(sensor + HW_GPIO_PIN_TEMP_SENSOR_INTERNAL),
            .humidity_integer = 0U,
            .humidity_decimal = 0U,
            .temperature_integer = 0U,
            .temperature_decimal = 0U,
            .valid = false
        };
        DRV_dht11_init(DRV_dht11_sensors[sensor]);
    }

    if(SYS_DEBUG_MODE) {
        printf("Hardware drivers initialised successfully.\n");
    }
}
