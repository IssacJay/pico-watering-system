/***********************************************************************************
** @file         cmd.c
** @brief        Command header file for the Pico Irrigation System.
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

/***********************************************************************************
** Includes
*/
#include <stdio.h>
#include "pico/stdlib.h"

#include "cmd.h"
#include "../drv/drv.h"
#include "../hw/hw.h"
#include "../sys/sys.h"


/***********************************************************************************
** Functions
*/
void CMD_start_watering(uint16_t water_pump) {
    DRV_relay_set_state(DRV_water_pump_relays[water_pump], true);
    gpio_put(HW_GPIO_PIN_LED, true);

    printf("Water pump %u started watering.\n", water_pump);
}

void CMD_stop_watering(uint16_t water_pump) {
    DRV_relay_set_state(DRV_water_pump_relays[water_pump], false);
    gpio_put(HW_GPIO_PIN_LED, false);

    printf("Water pump %u stopped watering.\n", water_pump);
}

float CMD_read_temperature(uint16_t *temperature_SENSOR) {
    if (temperature_SENSOR == NULL) {
        return -1; /* Invalid parameter */
    }

    uint16_t sensor_id = *temperature_SENSOR;
    if (sensor_id >= SYS_NUM_TEMP_SENSORS) {
        printf("Error: Invalid sensor ID %u for temperature read.\n", sensor_id);
        return -1;
    }

    /* Read from the DHT11 sensor */
    DRV_dht11_sensor_t sensor_info = DRV_dht11_sensors[sensor_id];
    DRV_dht11_sensor_t read_data;

    if (!DRV_dht11_read(sensor_info, &read_data)) {
        printf("Error: Failed to read temperature from sensor %u.\n", sensor_id);
        return -1;
    }

    if (!read_data.valid) {
        printf("Error: Temperature data from sensor %u is invalid.\n", sensor_id);
        return -1;
    }

    float temperature = read_data.temperature_integer + read_data.temperature_decimal;
    if(SYS_DEBUG_MODE) {
        printf("CMD_INFO: DHT11: Temperature from sensor %u: %u.%u°C (value: %f)\n", 
           sensor_id, read_data.temperature_integer, read_data.temperature_decimal, temperature);
    }
    
    return temperature;
}

float CMD_read_humidity(uint16_t *temperature_SENSOR) {
    if (temperature_SENSOR == NULL) {
        return -1; /* Invalid parameter */
    }

    uint16_t sensor_id = *temperature_SENSOR;
    if (sensor_id >= SYS_NUM_TEMP_SENSORS) {
    printf("CMD_ERR: DHT11: Invalid sensor ID %u for humidity read.\n", sensor_id);
        return -1;
    }

    /* Read from the DHT11 sensor */
    DRV_dht11_sensor_t sensor_info = DRV_dht11_sensors[sensor_id];
    DRV_dht11_sensor_t read_data;

    if (!DRV_dht11_read(sensor_info, &read_data)) {
        printf("CMD_ERR: DHT11: Failed to read humidity from sensor %u.\n", sensor_id);
        return -1;
    }

    if (!read_data.valid) {
        printf("CMD_ERR: DHT11: Humidity data from sensor %u is invalid.\n", sensor_id);
        return -1;
    }

    float humidity = read_data.humidity_integer + read_data.humidity_decimal;
    if(SYS_DEBUG_MODE) {
        printf("CMD_INFO: DHT11: Humidity from sensor %u: %u.%u%% (value: %f)\n", 
           sensor_id, read_data.humidity_integer, read_data.humidity_decimal, humidity);
    }
    
    return humidity;
}


void CMD_read_burst_switch() {
    uint16_t state = DRV_switch_get_state(DRV_switches[SYS_SWITCH_BURST]);
    if(state == GPIO_SWITCH_ACTIVE){
        printf("Burst switch triggered. Starting burst watering cycle for %u seconds...\n", SYS_WATERING_BURST_DURATION_SEC);
        sys_state = SYS_WATERING;
        CMD_start_watering(SYS_WATER_PUMP_TOP);
        gpio_put(HW_GPIO_PIN_LED, true); 
        sleep_ms(SEC_TO_MS(SYS_WATERING_BURST_DURATION_SEC));
        gpio_put(HW_GPIO_PIN_LED, false); 
        CMD_stop_watering(SYS_WATER_PUMP_TOP);
        sys_state = SYS_IDLE; 
    }
    return; 
}

void CMD_read_reset_switch() {
    uint16_t state = DRV_switch_get_state(DRV_switches[SYS_SWITCH_RESET]);
    if(state == GPIO_SWITCH_ACTIVE){
        sleep_ms(SEC_TO_MS(CMD_RESET_SWITCH_HOLD_DURATION_SEC));
        state = DRV_switch_get_state(DRV_switches[SYS_SWITCH_RESET]);
        if(state == GPIO_SWITCH_ACTIVE){
            printf("Reset switch triggered. Resetting system...\n");
            SYS_reset_elapsed_time();
            gpio_put(HW_GPIO_PIN_LED, true);
            sleep_ms(SEC_TO_MS(CMD_RESET_SWITCH_HOLD_DURATION_SEC));
            gpio_put(HW_GPIO_PIN_LED, false);
        }
    }
    return; 
}


