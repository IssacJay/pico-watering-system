/***********************************************************************************
** @file         sys_main.c
** @brief        Main entry point for the main function, initialising loop.
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

#include "sys.h"
#include "../hw/hw.h"

/***********************************************************************************
** Variables
*/
SYS_states   sys_state = SYS_BOOTING;
uint32_t     elapsed_time_ms = 0U;
uint32_t     watering_time_ms = 0U;

/***********************************************************************************
** Functions
*/
void main(void) {
    
    sys_state = SYS_BOOTING;

    /****************************/
    /* Initialise the hardware  */
    /****************************/
    HW_init();

    /****************************/
    /* Initialise the system    */
    /****************************/
    SYS_init();

    /****************************/
    /* Main Task Loop           */
    /****************************/
    SYS_task();
}
