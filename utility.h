/**
 * @file
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Utility module common structures
 * @version 0.1
 * @copyright MIT License.
 *
 * This module contains structs and functions that are used in a wide range of other modules and
 * serves as a key module.
 */
/**
 * @page utility Utility module
 * @tableofcontents
 *
 * @section secReset Reset
 * We reset the board with the help of the watchdog timer with a 5 second delay until reset. Once
 * we start the reset process the function will never left.
 *
 * \subsection subWatchdog Watchdog Timer
 * The watchdog timer is a software timer that is used to detect of the board malfunctions or is
 * stuck in an endless loop. If so the timer resets the board so it is no longer stuck nad instead
 * works as intended. This behaviour can be used by us to reset the board manually if we want so,
 * This is very easy first we set the timer to the wanted time until the reset and then we simply
 * enter an endless loop which never can be left and does nothing. The watchdog timer that thinks
 * the board is stuck, which is is in fact, and resets the board.
 */
#ifndef UTILITY
#define UTILITY

#include <avr/io.h>
#include <avr/wdt.h>
#include "led_control.h"

/**
 * @brief Amount of counters that are defined in #counter_def
 */
#define COUNTER_AMOUNT 8

/** @brief Contains parameters for a 5 second timer */
#define WATCH_DOG_TIME (WDTO_1S | WDTO_4S)

/**
 * @brief Helper struct that is used to check frequency requirements every cycle. For example this
 * is used to print a message every 1 HZ or every 5 HZ.
 * <p>
 * The amount of defined counters is #COUNTER_AMOUNT
 * @details All counters for the frequencies 1HZ, 5HZ and 8HZ are located in the enum #counter_def
 */
typedef struct counter {
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
} counter;

/**
 * @brief Defines the action state of the roboter
 */
typedef enum {
    /**
     * @brief The robot waits for instructions
     */
    AC_WAIT,
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
     * @brief The robot gets manual controlled
     */
    AC_MANUAL
} action_type;

/**
 * @brief Drive directions
 */
typedef enum {
    /** @brief No direction, don't drive */
    DIR_NONE,
    /** @brief Drive straight forward */
    DIR_FORWARD,
    /** @brief Turn right */
    DIR_RIGHT,
    /** @brief Turn left */
    DIR_LEFT,
    /** @brief Drive straight backward */
    DIR_BACK,
} direction;

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
 * @brief Shortly before start, recognised starting field and will start in next cycle.
 * @details This state is needed for calculation reasons
 */
    DS_ZERO_ROUND,
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
     * @brief Drive back until on start field again
     */
    DS_BACKWARDS,
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
     * @brief On the track before driving action was performed.
     */
    POS_UNKNOWN,
    /**
    * @brief On the start field.
    */
    POS_START_FIELD,
    /**
     * @brief On the track while driving.
     */
    POS_TRACK
} track_pos;

/**
 * @brief State of the connection to the user interface
 */
typedef enum {
    /**
    * @brief Not connected to the ui, do nothing
    */
    UI_DISCONNECTED,
    /**
    * @brief Currently connected to the ui, if this is the case we will send state updates to the ui
    */
    UI_CONNECTED
} ui_state;

/**
 * @brief Reflects the current inputs of the robot (sensors, position on the track), currently
 * performed action and last inputs.
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
     * @brief Caches the last three sensor states in pakages, the first three bits are the last
     * state, the next three bits the one before and so on
     * @brief States older than 3 cycles will simply be shifted out of the valid area
     */
    uint16_t sensor_brick;
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
    led_state last_led;
    /**
     * @brief Count of seconds of the robot on the board. A value from 0 to 2. If the robot is not
     * on the start field this is 0.
     */
    uint8_t homeCache;
    /**
     * @brief If the action state was activated since the last util_reset at least once.
     */
    uint8_t has_driven_once;

    /**
     * @brief Array / Pointer that contains the counters for all registered frequencies.
     */
    counter counters[COUNTER_AMOUNT];

    /**
     * @brief Direction that the manual control was given via serial
     */
    direction manual_dir;
    /**
     * @brief If the manuel mode drove in the last manual tick
     */
    uint8_t manual_dir_last;
    /**
     * @brief Last driven direction, used by ui state
     */
    direction last_dir;
    /**
     * @brief Last valid driven direction that was not #DIR_NONE
     */
    direction dir_last_valid;

    /**
     * @brief Connection state to the ui
     */
    ui_state ui_connection;
} track_state;

/**
 * @brief Resets the board after 5 seconds by using the watch dog timer.
 * @details For more information on the wdt look at p.76 of the datasheet
 */
_Noreturn void util_reset(void);

#endif //UTILITY
