/***********************************************************************************
** @file         drv_relay.c
** @brief        Relay driver source file for the Pico Irrigation System.
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
#include "../sys/sys.h"

/***********************************************************************************
 * Defines
 */
#define DRV_RELAY_ACTIVE   0
#define DRV_RELAY_INACTIVE 1

/***********************************************************************************
 * Variables
 */
DRV_relay_t DRV_water_pump_relays[SYS_NUM_WATER_PUMPS];

/***********************************************************************************
 * Functions
 */
void DRV_relay_set_state(DRV_relay_t relay, bool state) {
	uint8_t gpio_level = state ? GPIO_RELAY_ACTIVE : GPIO_RELAY_INACTIVE;
	DRV_water_pump_relays[relay.relay_id].state = state;
	gpio_put(relay.gpio_pin, gpio_level);
}

uint16_t DRV_relay_get_state(DRV_relay_t relay) {
	uint8_t gpio_level = gpio_get(relay.gpio_pin);
	uint8_t expected_level = DRV_water_pump_relays[relay.relay_id].state ? GPIO_RELAY_ACTIVE : GPIO_RELAY_INACTIVE;
	
	if(gpio_level != expected_level) {
		printf("DRV_WARN: RELAY: Relay state mismatch for relay ID %u. GPIO state does not match expected state.\n", relay.relay_id);
	}
	return DRV_water_pump_relays[relay.relay_id].state;
}