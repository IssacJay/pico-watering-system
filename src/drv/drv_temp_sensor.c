/***********************************************************************************
** @file         drv_temp_sensor.c
** @brief        DHT11 temperature and humidity sensor driver for the Pico
**               Irrigation System.
** @author       Issac Thomas
** @date         07 May 2026
** Project       Pico Irrigation System
************************************************************************************
** Revisions:
** V1.0 - 07 May 2026 - Initial version 
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
#include "hardware/gpio.h"

#include "drv.h"
#include "../hw/hw.h"

/***********************************************************************************
 *  Defines
 */
#define DHT11_START_SIGNAL_MS       20U     /* Requirements state line low for at least 18ms */
#define DHT11_START_WAIT_US         30U     /* Wait after release for DHT response (20-40us) */
#define DHT11_RESPONSE_TIMEOUT_US   150U    /* Max wait for DHT11 80us response + margin */
#define DHT11_BIT_LOW_TIMEOUT_US    100U    /* Max wait for bit low pulse */
#define DHT11_BIT_HIGH_TIMEOUT_US   100U    /* Max wait for bit high pulse */
#define DHT11_BIT_THRESHOLD_US      40U     /* >40us high = '1', <40us = '0' (26-28us=0, 70us=1) */
#define DHT11_DATA_BITS             40U     /* 5 bytes = 40 bits */
#define DHT11_MIN_READ_INTERVAL_MS  1000U   /* DHT11 requires 1s between reads */

/***********************************************************************************
 *  Variables
 */
DRV_dht11_sensor_t DRV_dht11_sensors[HW_GPIO_PIN_TEMP_SENSOR_NUM_PINS];
static uint32_t last_read_time_ms[HW_GPIO_PIN_TEMP_SENSOR_NUM_PINS] = {0};
static uint32_t cache_valid_time_ms = 200U;  /* Keep cache valid for 200ms to allow temp+humidity read */

/***********************************************************************************
 *  Static Functions
 */

/***********************************************************************************
** @brief Wait for a specific GPIO level with a timeout.
** @param gpio_pin The GPIO pin to monitor.
** @param level The expected level (true = high, false = low).
** @param timeout_us Timeout in microseconds.
** @return true if the level was detected, false if timed out.
************************************************************************************/
static bool DRV_dht11_wait_for_level(uint16_t gpio_pin, bool level, uint32_t timeout_us) {
    uint32_t elapsed = 0;
    while (gpio_get(gpio_pin) == (level ? 0 : 1)) {
        busy_wait_us_32(1);
        elapsed++;
        if (elapsed > timeout_us) {
            return false;
        }
    }
    return true;
}

/***********************************************************************************
** @brief Measure the duration of a GPIO level in microseconds.
** @param gpio_pin The GPIO pin to monitor.
** @param level The level to measure (true = high, false = low).
** @param timeout_us Timeout in microseconds.
** @return Duration in microseconds, or 0 if timed out.
************************************************************************************/
static uint32_t DRV_dht11_measure_pulse(uint16_t gpio_pin, bool level, uint32_t timeout_us) {
    uint32_t duration = 0;
    while (gpio_get(gpio_pin) == (level ? 1 : 0)) {
        busy_wait_us_32(1);
        duration++;
        if (duration > timeout_us) {
            return 0;
        }
    }
    return duration;
}

/***********************************************************************************
 *  Public Functions
 */

/***********************************************************************************
** @brief Read raw data from a DHT11 sensor.
** @param sensor The sensor object.
** @param data Pointer to the data structure to populate.
** @return true if the read was successful, false otherwise.
************************************************************************************/
bool DRV_dht11_read(DRV_dht11_sensor_t sensor, DRV_dht11_sensor_t *data) {
    
    uint8_t raw_data[5] = {0};
    uint16_t gpio_pin = sensor.gpio_pin;
    uint32_t current_time_ms = time_us_32() / 1000U;

    if (data == NULL) {
        return false;
    }

    if (sensor.sensor_id >= HW_GPIO_PIN_TEMP_SENSOR_NUM_PINS) {
        data->valid = false;
        return false;
    }

    /* Enforce DHT11 read interval constraint: minimum 1 second between consecutive reads.
     * On Pico, this is critical because the GPIO may not fully recover between rapid reads.
     * Datasheet Section 6: "Sampling period at intervals should be no less than 1 second."
     * 
     * OPTIMIZATION: If the cache is still valid (within 200ms), return cached data instead.
     * This allows fast access to both temperature and humidity without hitting the 1s interval.
     * Only NEW sensor reads (not cached) require the 1s spacing.
     */
    uint32_t time_since_last_read = current_time_ms - last_read_time_ms[sensor.sensor_id];
    
    /* Check if we have valid cached data within 200ms window */
    if (time_since_last_read < cache_valid_time_ms && DRV_dht11_sensors[sensor.sensor_id].valid) {
        /* Return cached data - this is the fast path for temp+humidity on same sensor */
        *data = DRV_dht11_sensors[sensor.sensor_id];
        return true;
    }
    
    /* Cache expired or invalid - check if we can do a new read */
    if (time_since_last_read < DHT11_MIN_READ_INTERVAL_MS) {
        printf("DHT11: Read interval too short on GPIO %u (need %ums between new reads, cache expired)\n", 
               gpio_pin, DHT11_MIN_READ_INTERVAL_MS);
        data->valid = false;
        return false;
    }

    /* Send start signal per datasheet Section 5.2:
     * 1. MCU pulls data line LOW for at least 18ms
     * 2. MCU releases (pulls HIGH) and waits 20-40us
     * 3. DHT11 responds by pulling LOW for ~80us, then HIGH for ~80us
     */
    gpio_set_dir(gpio_pin, GPIO_OUT);
    gpio_put(gpio_pin, 0);                          /* Pull LOW */
    sleep_ms(DHT11_START_SIGNAL_MS);                /* Hold for 18ms minimum */
    
    /* Release the line by switching to INPUT (high impedance).
     * This allows DHT11 to pull it LOW for its response signal.
     */
    gpio_set_dir(gpio_pin, GPIO_IN);
    gpio_pull_up(gpio_pin);                         /* Ensure pull-up is active */
    busy_wait_us_32(DHT11_START_WAIT_US);           /* Wait 20-40us for DHT to respond */

    /* Wait for DHT11 response: pulls data line LOW for ~80us */
    if (!DRV_dht11_wait_for_level(gpio_pin, false, 100U)) {
        printf("DRV_ERR: DHT11: No response (low) on GPIO %u!\n", gpio_pin);
        /* Ensure GPIO is reset to safe state on failure */
        DRV_dht11_init(sensor);  /* Reset sensor */
        return false;
    }

    /* Wait for DHT11 to release: data line goes HIGH for ~80us (ready to send data) */
    if (!DRV_dht11_wait_for_level(gpio_pin, true, 100U )) {
        printf("DRV_ERR: DHT11: No response (high) on GPIO %u!\n", gpio_pin);
        /* Ensure GPIO is reset to safe state on failure */
        DRV_dht11_init(sensor);  /* Reset sensor */
        return false;
    }

    /* Read 40 bits of data per datasheet Section 5.3:
     * Each bit starts with ~50us LOW pulse, followed by HIGH pulse.
     * HIGH duration determines bit value: 26-28us=0, 70us=1 (threshold ~40us)
     */
    for (uint8_t i = 0; i < DHT11_DATA_BITS; i++) {
        /* Wait for bit LOW phase (~50us) - marks the start of a new bit */
        if (!DRV_dht11_wait_for_level(gpio_pin, false, DHT11_BIT_LOW_TIMEOUT_US)) {
            printf("DRV_ERR: DHT11: Bit %u LOW timeout on GPIO %u!\n", i, gpio_pin);
            DRV_dht11_init(sensor);  /* Reset sensor */
            return false;
        }

        /* Wait for bit HIGH phase to begin */
        if (!DRV_dht11_wait_for_level(gpio_pin, true, DHT11_BIT_HIGH_TIMEOUT_US)) {
            printf("DRV_ERR: DHT11: Bit %u HIGH timeout on GPIO %u!\n", i, gpio_pin);
            DRV_dht11_init(sensor);  /* Reset sensor */
            return false;
        }

        /* Measure HIGH pulse duration to determine bit value */
        uint32_t high_duration = DRV_dht11_measure_pulse(gpio_pin, true, DHT11_BIT_HIGH_TIMEOUT_US);
        if (high_duration == 0) {
            printf("DRV_ERR: DHT11: Bit %u measure timeout on GPIO %u!\n", i, gpio_pin);
            DRV_dht11_init(sensor);  /* Reset sensor */
            return false;
        }

        /* Shift in the bit: >40us = '1', <40us = '0' */
        raw_data[i / 8] <<= 1;
        if (high_duration > DHT11_BIT_THRESHOLD_US) {
            raw_data[i / 8] |= 1;
        }
    }

    /* Ensure GPIO returns to safe INPUT state after successful data reception */
    gpio_set_dir(gpio_pin, GPIO_IN);
    gpio_pull_up(gpio_pin);

    /* Verify checksum: sum of first 4 bytes should equal the 5th byte */
    uint8_t checksum = raw_data[0] + raw_data[1] + raw_data[2] + raw_data[3];
    if (checksum != raw_data[4]) {
        printf("DRV_ERR: DHT11: Checksum failed on GPIO %u (expected 0x%02X, got 0x%02X)!\n",
               gpio_pin, checksum, raw_data[4]);
        DRV_dht11_init(sensor);  /* Reset sensor */
        return false;
    }

    /* Parse and cache data */
    DRV_dht11_sensors[sensor.sensor_id].humidity_integer    = raw_data[0];
    DRV_dht11_sensors[sensor.sensor_id].humidity_decimal    = raw_data[1];
    DRV_dht11_sensors[sensor.sensor_id].temperature_integer = raw_data[2];
    DRV_dht11_sensors[sensor.sensor_id].temperature_decimal = raw_data[3];
    DRV_dht11_sensors[sensor.sensor_id].valid               = true;

    *data = DRV_dht11_sensors[sensor.sensor_id];
    last_read_time_ms[sensor.sensor_id] = current_time_ms;

    return true;
}

/***********************************************************************************
** @brief Initialise a DHT11 sensor GPIO pin with pull-up.
** @param sensor The sensor object.
************************************************************************************/
void DRV_dht11_init(DRV_dht11_sensor_t sensor) {
    if (sensor.sensor_id >= HW_GPIO_PIN_TEMP_SENSOR_NUM_PINS) {
        return;
    }

    gpio_init(sensor.gpio_pin);
    gpio_set_dir(sensor.gpio_pin, GPIO_IN);
    gpio_pull_up(sensor.gpio_pin);

    DRV_dht11_sensors[sensor.sensor_id] = sensor;
    DRV_dht11_sensors[sensor.sensor_id].valid = false;
}

/***********************************************************************************
** @brief Get the last successful reading for a sensor.
** @param sensor The sensor object.
** @return Pointer to the sensor data structure.
************************************************************************************/
DRV_dht11_sensor_t* DRV_dht11_get_data(DRV_dht11_sensor_t sensor) {
    if (sensor.sensor_id >= HW_GPIO_PIN_TEMP_SENSOR_NUM_PINS) {
        return NULL;
    }
    return &DRV_dht11_sensors[sensor.sensor_id];
}
