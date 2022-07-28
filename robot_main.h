/**
 * @file robot_main.h
 * @author Larson Schneider
 * @date 26.07.2022
 * @brief Main module of the robot system
 * @version 0.1
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
 */
void setup(void);

/**
 * @brief Main method, entry point of the program
 */
int main(void);

#endif
