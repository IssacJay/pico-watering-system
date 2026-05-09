/***********************************************************************************
** @file         hw.c
** @brief        Hardware initialisation source file (GPIO, I2C, SPI, etc.).
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
 * Includes
 */

#include <stdio.h>
#include "pico/stdlib.h"

#include "hw.h"
#include "../drv/drv.h"
#include "../sys/sys.h"

/***********************************************************************************
** Functions
*/

void HW_init(void) {

    
    /****************************/
    /* Initialise Serial Port (USB/UART) */
    /****************************/
    stdio_init_all();
    sleep_ms(10000);          /* Wait 10sec To Allow USB Serial Port Comms To Be Established Be Recieving Device */

    /****************************/
    /* Initialise the Hardware Drivers */
    /****************************/  
    DRV_init();

    /****************************/
    /* Initialise GPIO Pins */
    /****************************/
    /* 
    ** Initialise GPIOs
    */
    gpio_init(HW_GPIO_PIN_LED);
    for (uint16_t relay = 0; relay < HW_GPIO_PIN_RELAY_NUM_PINS; relay++) {
        gpio_init(DRV_water_pump_relays[relay].gpio_pin);
    }

    /*
    ** Set GPIO Outputs
    */
    gpio_set_dir(HW_GPIO_PIN_LED, GPIO_OUT);
    for (uint16_t relay = 0; relay < HW_GPIO_PIN_RELAY_NUM_PINS; relay++) {
        gpio_set_dir(DRV_water_pump_relays[relay].gpio_pin, GPIO_OUT);
    }
    
    if(SYS_DEBUG_MODE) {
        printf("GPIO pins initialised successfully.\n");
    }


    /****************************/
    /* Hello World */
    /****************************/
    gpio_put(HW_GPIO_PIN_LED, 1);
    for (uint16_t relay = 0; relay < HW_GPIO_PIN_RELAY_NUM_PINS; relay++) {
        gpio_put(DRV_water_pump_relays[relay].gpio_pin, GPIO_RELAY_ACTIVE);
    }
    sleep_ms(2000);         /* Turn on LED for 2sec to show device is ready */
    gpio_put(HW_GPIO_PIN_LED, 0);
    for (uint16_t relay = 0; relay < HW_GPIO_PIN_RELAY_NUM_PINS; relay++) {
        gpio_put(DRV_water_pump_relays[relay].gpio_pin, GPIO_RELAY_INACTIVE);
    }
    if(SYS_DEBUG_MODE) {
        printf("Hello, from the Pico Irrigation System!\n");
    }
}
