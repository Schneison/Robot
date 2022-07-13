/**
 * @file state_control.h
 * @author Larson Schneider
 * @date 09.07.2022
 * @brief Utility class
 * @version 0.1
 */
#ifndef BRAIN_H
#define BRAIN_H

#include <avr/wdt.h>
#include "robot_sensor.h"
#include "timers.h"

/**
 * @brief Resets the board after 5 seconds by using the watch dog timer.
 * @details For more information on the wdt look at p.76 of the datasheet
 */
void reset(void);


#endif
