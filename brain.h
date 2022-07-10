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

/**
 * @brief Current state of the driving action.
 */
typedef enum {
    /**
     * @brief Before driving action was selected
     */
    PRE_DRIVE,
    /**
     * @brief After action was selected and home field is not selected yet
     */
    CHECK_START,
    /**
     * @brief Started an currently driving the first round.
     */
    FIRST_ROUND,
    /**
     * @brief Finished first round, currently in second round.
     */
    SECOND_ROUND,
    /**
    * @brief Finished second round, currently in third and last round.
    */
    THIRD_ROUND,
    /**
     * @brief Finished driving, try to reset robot.
     */
    POST_DRIVE,

} drive_state;

/**
 * @brief Position on the field.
 */
typedef enum {
    /**
    * @brief On the start field.
    */
    POS_START_FIELD,
    /**
     * @brief On the track while driving.
     */
    POS_TRACK,
    /**
     * @brief On the track before driving action was performed.
     */
    POS_UNKNOWN
} track_pos;

/**
 * @brief Reflects the current inputs of the robot (sensors, position on the track), currently performed action and last
 * inputs.
 */
typedef struct track_state {
    /**
     * @brief Currently performed action
     */
    action_state action;
    /**
     * @brief Current state of the drive action, if the action is performed currently.
     */
    drive_state drive;
    /**
     * @brief State of the sensors on the last tick
     */
    sensor_state sensor_last;
    /**
     * @brief Position of the robot on the track
     */
    track_pos pos;
    /**
     * @brief Last position of the robot on the track
     */
    track_pos last_pos;
    /**
     * @brief Count of seconds of the robot on the board. A value from 0 to 2. If the robot is not on the start field
     * this is 0.
     */
    uint8_t homeCache;

} track_state;


#endif
