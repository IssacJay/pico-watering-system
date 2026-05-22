/***********************************************************************************
** @file         sys_task.c
** @brief        Main task in program, running the main loop.
** @author       Issac Thomas
** @date         02 May 2026
** Project       Pico Irrigation System
************************************************************************************
** Revisions:
** V1.1 - 22 May 2026 - Adjusted watering period (12hrs)
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
#include "../cmd/cmd.h"

/***********************************************************************************
 ** Variables
 */


/***********************************************************************************
 ** Functions
 */
void SYS_task(void) {
    if(SYS_DEBUG_MODE) {
        printf("System task started. Initial state: SYS_IDLE\nEntering main task loop...\n");
    }

    while (true) {
        switch (sys_state) {
            case SYS_IDLE:
                break;
            case SYS_WATERING:
                /* Sequenct each water pump */
                for (uint16_t water_pump = 0; water_pump < SYS_NUM_WATER_PUMPS; water_pump++) {
                    if(SYS_DEBUG_MODE) {
                        printf("Starting watering cycle for water pump %u...\n", water_pump);
                    }
                    CMD_start_watering(water_pump);
                    sleep_ms(SEC_TO_MS(SYS_WATERING_DURATION_SEC));
                    CMD_stop_watering(water_pump);
                    printf("Completed watering cycle for water pump %u.\n", water_pump);
                }
                sys_state = SYS_IDLE;
                break;
            case SYS_ERROR:
            default:
                /* @todo Implement error handling */
                break;
        }

        if(elapsed_time_ms >= HRS_TO_MS(SYS_WATERING_INTERVAL_HRS) && sys_state == SYS_IDLE) {
            printf("Starting watering cycle for %u seconds...\n", SYS_WATERING_DURATION_SEC);
            SYS_reset_elapsed_time();
            sys_state = SYS_WATERING;
        }

        if(elapsed_time_ms % SEC_TO_MS(SYS_TEMPERATURE_INTERVAL_SEC) == 0) {
            printf("Reading temperature and humidity sensors...\n");
            uint16_t sensor_id = SYS_TEMP_SENSOR_INTERNAL;
            float temp = CMD_read_temperature(&sensor_id); 
            float humidity = CMD_read_humidity(&sensor_id);  
            printf("Sensor %u - Temperature: %.1f°C, Humidity: %.1f%%\n", sensor_id, temp, humidity);
        }

        elapsed_time_ms += SYS_TASK_LOOP_DELAY_MS;
        sleep_ms(SYS_TASK_LOOP_DELAY_MS); 
    }
}