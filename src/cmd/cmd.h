/***********************************************************************************
** @file         cmd.h
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

#ifndef CMD_H_
#define CMD_H_

/***********************************************************************************
** Includes
*/
#include <stdint.h>

/***********************************************************************************
** Functions
*/


/** @brief Turn on a specific water pump in the system.
 *  @param water_pump The water pump to start watering.
 */
void CMD_start_watering(uint16_t water_pump);

/** @brief Turn off a specific water pump in the system.
 *  @param water_pump The water pump to stop watering.
 */
void CMD_stop_watering(uint16_t water_pump);

/** @brief Read the temperature from the temperature/humidity sensors. */
float CMD_read_temperature(uint16_t *temperature_SENSOR);

/** @brief Read the humidity from the temperature/humidity sensors. */
float CMD_read_humidity(uint16_t *temperature_SENSOR);





#endif /* CMD_H_ */
