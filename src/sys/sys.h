/***********************************************************************************
** @file         sys.h
** @brief        System header file for the Pico Irrigation System.
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

#ifndef SYS_H_
#define SYS_H_

/***********************************************************************************
** Includes
*/
#include <stdint.h>
#include "../drv/drv.h"

/***********************************************************************************
** Macros
*/
#define MS_TO_SEC(x)                        ((x) / 1000U)
#define SEC_TO_MS(x)                        ((x) * 1000U)
#define MS_TO_HRS(x)                        ((x) / 3600000U)
#define HRS_TO_MS(x)                        ((x) * 3600000U)

/***********************************************************************************
** Defines
*/
#define SYS_VERSION_MAJOR                   1U
#define SYS_VERSION_MINOR                   0U
#define SYS_VERSION_BUILD                   0U

#define SYS_DEBUG_MODE                      0U

#ifdef _PICO_W
    #define SYS_DEVICE_NAME                     "Wireless Pico Irrigation System"
#else
    #define SYS_DEVICE_NAME                     "Pico Irrigation System"
#endif /* _PICO_W */

#define SYS_TASK_LOOP_DELAY_MS              1U
#define SYS_WATERING_DURATION_SEC           20U
#define SYS_WATERING_BURST_DURATION_SEC     5U
#define SYS_WATERING_INTERVAL_HRS           12U
#define SYS_TEMPERATURE_INTERVAL_SEC        10U


typedef enum {
    SYS_WATER_PUMP_TOP,
    SYS_WATER_PUMP_BOTTOM,
    SYS_NUM_WATER_PUMPS
} SYS_water_pumps_t;

typedef enum {
    SYS_TEMP_SENSOR_INTERNAL,
    SYS_TEMP_SENSOR_EXTERNAL,
    SYS_NUM_TEMP_SENSORS
} SYS_temp_sensors_t;

typedef enum { 
    SYS_SWITCH_BURST,
    SYS_SWITCH_RESET,
    SYS_NUM_SWITCHES
} SYS_switches_t; 

/* @brief System states 
*/
typedef enum {
    SYS_BOOTING,
    SYS_IDLE,
    SYS_WATERING,
    SYS_WATERING_COOLING,
    SYS_ERROR,
    SYS_NUM_STATES
} SYS_states;

extern SYS_states   sys_state;
extern uint32_t     elapsed_time_ms;
extern uint32_t     watering_time_ms;


/***********************************************************************************
** Functions
*/

/***********************************************************************************
** Main Prototypes
*/
/***********************************************************************************
** @brief Main entry point for the main function, initialising loop.
************************************************************************************/
void main(void);

/***********************************************************************************
** Init Prototypes
*/

/** @brief N/A
*/
void SYS_init(void);

/** @brief Resets the elapsed background task counter.
 */
void SYS_reset_elapsed_time(void);

/** @brief Retrieves the current elapsed time of background task in milliseconds.
 *  @return Elapsed time in milliseconds
 */
uint32_t SYS_get_elapsed_time(void);

/***********************************************************************************
** Task Prototypes
*/
/***********************************************************************************
** @brief Main background task, with time counter and scheduler.
************************************************************************************/
void SYS_task(void);

#endif /* SYS_H_ */
/***********************************************************************************
** End Of File
*/