/**
 * @file brain.h
 * @author Larson Schneider
 * @date 09.07.2022
 * @brief Utility class
 * @version 0.1
 */
#ifndef BRAIN_H
#define BRAIN_H

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
    RETURN_HOME
} State;


#endif
