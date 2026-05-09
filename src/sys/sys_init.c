/***********************************************************************************
** @file         sys_init.c
** @brief        System initialisation functions.
** @author       Issac Thomas
** @date         01 May 2026
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
** Includes
*/
#include <stdio.h>
#include "pico/stdlib.h"

#include "sys.h"
#include "../hw/hw.h"
#include "../drv/drv.h"

/***********************************************************************************
** Functions
*/
void SYS_init(void) {
    /* Initialise System State */
    sys_state = SYS_IDLE;
    
    /* Reset Elapsed Time Counter */
    SYS_reset_elapsed_time();

    if(SYS_DEBUG_MODE) {
        printf("System initialisation complete. Device is now idle.\n");
    }
}

void SYS_reset_elapsed_time(void) {
    elapsed_time_ms = 0U;
}

uint32_t SYS_get_elapsed_time(void) {
    return elapsed_time_ms;
}

