/**
 * @file utility.h
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Utility class
 * @version 0.1
 */
#ifndef UTILITY
#define UTILITY

#include <avr/io.h>
#include "led_control.h"
#include <avr/wdt.h>

/**
 * @brief Resets the board after 5 seconds by using the watch dog timer.
 * @details For more information on the wdt look at p.76 of the datasheet
 */
void util_reset(void);

/**
 * @brief Amount of counters that are defined in #counter_def
 */
#define COUNTER_AMOUNT 7

/**
 * @brief Helper struct that is used to check frequency requirements every cycle. For example this is used to print a
 * message every 1 HZ or every 5 HZ.
 * <p>
 * The amount of defined counters is #COUNTER_AMOUNT
 * @details All counters for the frequencies 1HZ, 5HZ and 8HZ are located in the enum #counter_def
 */
typedef struct Counter {
    /**
     * @brief Milliseconds since last true cycle.
     */
    uint16_t lastMillis;
    /**
     * @brief If the frequency applies this cycle. Possible values are 0 and 1.
     */
    uint8_t value;
    /**
     * @brief Frequency threshold
     */
    uint16_t threshold;
} Counter;

/**
 * @brief Defines the action state of the roboter
 */
typedef enum {
/**
 * @brief The roboter drives 3 rounds from the start point and resets after this.
 */
    AC_ROUNDS,
/**
 * @brief The roboter waits 5 seconds and resets to default state
 */
    AC_RESET,
/**
 * @brief The roboter makes a pause
 */
    AC_PAUSE,
/**
 * @brief The roboter reacts to nothing until a hard util_reset is done
 */
    AC_FROZEN,
/**
 * @brief The robot drives back home and resets itself there
 */
    AC_RETURN_HOME,
/**
 * @brief The robot waits for instructions
 */
    AC_WAIT,
} action_type;

/**
 * @brief Current state of the driving action.
 */
typedef enum {
    /**
     * @brief Before driving action was selected
     */
    DS_PRE_DRIVE,
    /**
     * @brief After action was selected and home field is not selected yet
     */
    DS_CHECK_START,
    /**
     * @brief Started an currently driving the first round.
     */
    DS_FIRST_ROUND,
    /**
     * @brief Finished first round, currently in second round.
     */
    DS_SECOND_ROUND,
    /**
    * @brief Finished second round, currently in third and last round.
    */
    DS_THIRD_ROUND,
    /**
     * @brief Finished driving, try to util_reset robot.
     */
    DS_POST_DRIVE,

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
    action_type action;
    /**
     * @brief Current state of the drive action, if the action is performed currently.
     */
    drive_state drive;
    /**
     * @brief State of the sensors on the last tick
     */
    sensor_state sensor_last;
    /**
 * @brief State of the sensors on the this tick
 */
    sensor_state sensor_current;
    /**
     * @brief Position of the robot on the track
     */
    track_pos pos;
    /**
     * @brief Last position of the robot on the track
     */
    track_pos last_pos;
    /**
     * @brief Last state of the chase light
     */
    LED_State last_led;
    /**
     * @brief Count of seconds of the robot on the board. A value from 0 to 2. If the robot is not on the start field
     * this is 0.
     */
    uint8_t homeCache;
    /**
     * @brief If the action state was activated since the last util_reset at least once.
     */
    uint8_t has_driven_once;

    /**
     * @brief Array / Pointer that contains the counters for all registered frequencies.
     */
    Counter counters[COUNTER_AMOUNT];
} track_state;


#endif //UTILITY
