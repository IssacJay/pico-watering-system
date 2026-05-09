/***********************************************************************************
** @file         drv.h
** @brief        Hardware Driver header file for the Pico Irrigation System.
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

#ifndef DRV_H_
#define DRV_H_

 /***********************************************************************************
 *  Includes
 */
#include <stdint.h>
#include <stdbool.h>

#include "../hw/hw.h"

 /***********************************************************************************
 *  Defines
 */
 
typedef struct {
    uint16_t relay_id;
    uint16_t gpio_pin;
    bool state;
} DRV_relay_t;

typedef struct {
    uint16_t sensor_id;
    uint16_t gpio_pin; 
    uint8_t humidity_integer;
    uint8_t humidity_decimal;
    uint8_t temperature_integer;
    uint8_t temperature_decimal;
    bool valid;
} DRV_dht11_sensor_t;

extern DRV_relay_t      DRV_water_pump_relays[];
extern DRV_dht11_sensor_t DRV_dht11_sensors[];

/***********************************************************************************
 * Functions
 */

/* @brief Initialise the hardware drivers 
 */
void DRV_init(void);


/* @brief Sets the state of the relay.
 * @param relay The relay to control.
 * @param state The desired state of the relay (true for ON, false for OFF).
 */
void DRV_relay_set_state(DRV_relay_t relay, bool state);

/* @brief Gets the state of the relay.
 * @param relay The relay to query.
 * @return The current state of the relay (true for ON, false for OFF).
 */
uint16_t DRV_relay_get_state(DRV_relay_t relay);

/* @brief Initialise a DHT11 sensor GPIO pin.
 * @param sensor The sensor object.
 */
void DRV_dht11_init(DRV_dht11_sensor_t sensor);

/* @brief Read temperature and humidity from a DHT11 sensor.
 * @param sensor The sensor object.
 * @param data Pointer to the data structure to populate.
 * @return true if the read was successful, false otherwise.
 */
bool DRV_dht11_read(DRV_dht11_sensor_t sensor, DRV_dht11_sensor_t *data);

/* @brief Get the last successful reading for a sensor.
 * @param sensor The sensor object.
 * @return Pointer to the sensor data structure.
 */
DRV_dht11_sensor_t* DRV_dht11_get_data(DRV_dht11_sensor_t sensor);

#endif /* DRV_H_ */