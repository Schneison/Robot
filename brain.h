/**
 * @file brain.h
 * @author Larson Schneider
 * @date 09.07.2022
 * @brief Utility class
 * @version 0.1
 */
#ifndef BRAIN_H
#define BRAIN_H

#include <avr/wdt.h>
#include <avr/sleep.h>
#include "robot_sensor.h"

/**
 * @brief Resets the board after 5 seconds by using the watch dog timer.
 * @details For more information on the wdt look at p.76 of the datasheet
 */
void reset(void);

/**
 * @brief Defines the action state of the roboter
 */
typedef enum {
/**
 * @brief The roboter drives 3 rounds from the start point and resets after this.
 */
    ROUNDS,
/**
 * @brief The roboter waits 5 seconds and resets to default state
 */
    RESET,
/**
 * @brief The roboter makes a pause
 */
    PAUSE,
/**
 * @brief The roboter reacts to nothing until a hard reset is done
 */
    FROZEN,
/**
 * @brief The robot drives back home and resets itself there
 */
    RETURN_HOME,
/**
 * @brief The robot waits for instructions
 */
    WAIT,
} action_state;

typedef enum {
    PRE_DRIVE,
    CHECK_START,
    ON_TRACK,
    FIRST_ROUND,
    SECOND_ROUND,
    THIRD_ROUND,
    POST_DRIVE,

} drive_state;

typedef enum {
    POS_START_FIELD,
    POS_TRACK,
    POS_UNKNOWN
} track_pos;

typedef struct track_state {
    action_state action;
    drive_state drive;
    sensor_state sensor_last;
    track_pos pos;
    uint8_t homeCache;

} track_state;


#endif
