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

/**
 * @brief Amount of counters that are defined in #counter_def
 */
#define COUNTER_AMOUNT 4

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
 * @brief Describes the binary state of the sensors
 */
typedef enum {
/**
 * @brief Left sensor is high
 */
    SENSOR_LEFT = 1,
/**
 * @brief Center sensor is high
 */
    SENSOR_CENTER = 2,
/**
 * @brief Right sensor is high
 */
    SENSOR_RIGHT = 4,
/**
 * @brief All sensors are high
 */
    SENSOR_ALL = 7,
} sensor_state;

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

    /**
     * @brief Array / Pointer that contains the counters for all registered frequencies.
     */
    Counter counters[COUNTER_AMOUNT];
} track_state;


#endif //UTILITY
