/**
 * @file
 * @author Larson Schneider
 * @date 26.07.2022
 * @brief Main module and try point of the robot system
 * @version 0.1
 * @copyright MIT License.
 *
 * Used to setup the registers and then pass the work on to the state control to do the main work
 * and cycles.
 */

#ifndef ROBOT_MAIN_H
#define ROBOT_MAIN_H

#include <stdlib.h>

#include "timers.h"
#include "usart.h"
#include "robot_sensor.h"
#include "drive_control.h"
#include "state_control.h"
#include "led_control.h"

/**
 * @brief Setup board registries
 *
 * First calls all existing clear methods to reset all needed registers and then all
 * init methods from the other modules.
 *
 * @sa usart_init
 * @sa motor_init
 * @sa led_init
 * @sa timers_init
 */
void setup(void);

/**
 * @brief Main method, entry point of the program
 *
 * Foremost all registers are cleared and newly setup with @ref setup next creates the global state
 * and calls the state control module to start work.
 *
 */
int main(void);

#endif
