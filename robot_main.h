/**
 * @file
 * @author Larson Schneider
 * @date 26.07.2022
 * @brief Main module and try point of the robot system
 * @version 0.1
 *
 * Used to setup the registers and then pass the work on to the state control to do the main work
 * and cycles.
 */

/**
 * @page modules Modules
 * @tableofcontents
 * The structure of this project is divided into multiple modules each of which takes on different
 * tasks like driving, state control or reading from the sensors.
 * @section secModAll All Modules
 * - @subpage states
 * - @subpage drive
 * - @subpage sensor
 * - @subpage timers
 * - @subpage utility
 * - @subpage usart
 * - @subpage led
 *
 * @section secModSet Setup
 * A module contains functions, macro defs, structs and enums. All methods are prefixed with the
 * name of respective module.@n
 * Some but not all modules contain "clean" and "init" (setup) functions, former sets some used registers
 * to there default values, latter sets these registers to the value needed for the robot to
 * function correctly. @n
 * The method @ref setup call all these before the main programm relly starts with the work.
 */

#ifndef ROBOT_MAIN_H
#define ROBOT_MAIN_H

#include "timers.h"
#include "usart.h"
#include "robot_sensor.h"
#include "drive_control.h"
#include "state_control.h"
#include "led_control.h"
#include <stdlib.h>

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
