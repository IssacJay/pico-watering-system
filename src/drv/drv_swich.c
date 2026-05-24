/***********************************************************************************
** @file         drv_switch.c
** @brief        Momentary Switch source file for the Pico Irrigation System.
** @author       Issac Thomas
** @date         22 May 2026
** Project       Pico Irrigation System
************************************************************************************
** Revisions:
** V1.0 - 22 May 2026 - Initial version 
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

#define DRV_SWITCH_DEBOUNCE_DELAY_MS    5U

/***********************************************************************************
 * Variables
 */
DRV_switch_t DRV_switches[SYS_NUM_SWITCHES];


/***********************************************************************************
 * Functions
 */


uint16_t DRV_switch_debounce(DRV_switch_t pole_switch) {
	uint16_t state = 0;
	if (pole_switch.switch_id < SYS_NUM_SWITCHES) {
		uint8_t first_level = gpio_get(pole_switch.gpio_pin);
		
		sleep_ms(DRV_SWITCH_DEBOUNCE_DELAY_MS);
		uint8_t second_level = gpio_get(pole_switch.gpio_pin);
				
		if (second_level == first_level) {
			/* Inputs use internal pull-ups (active-low) */ 
			state = first_level ? GPIO_SWITCH_INACTIVE : GPIO_SWITCH_ACTIVE;
			DRV_switches[pole_switch.switch_id].state = state;
		} else {
			state = DRV_switches[pole_switch.switch_id].state;
		}
	}
	else {
		printf("DRV_SWITCH: INVALID Switch ID.\n"); 
	}
	return state;
}

uint16_t DRV_switch_get_state(DRV_switch_t pole_switch) {
	/* Return a debounced reading and update internal state */
	return DRV_switch_debounce(pole_switch);
}

