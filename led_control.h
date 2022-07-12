/**
 * @file led_control.h
 * @author Larson Schneider
 * @date 09.07.2022
 * @brief Utility class for led functions
 * @version 0.1
 */
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <avr/io.h>

/**
 * @brief LED shift register input pin
 */
#define LED PB2
/**
 * @brief LED shift register clock flag pin
 */
#define LED_CLOCK PD4
/**
 * @brief Amount of led on this roboter
 * @details Defines how many shifts have to be done to update all led values
 */
#define LED_AMOUNT 3

/**
 * @brief Describes the binary state that is needed to activate the LED'S
 */
typedef enum {
/**
 * @brief Activates no LED
 */
    LED_NONE = 0,
/**
 * @brief Activates only the right LED
 */
    LED_RIGHT = 1,
/**
 * @brief Activates only the center LED
 */
    LED_CENTER = 2,
/**
 * @brief Activates only the left LED
 */
    LED_LEFT = 4,
/**
 * @brief Activates only the center LED'S
 */
    LED_ALL = 7,
} LED_State;

/**
 * @brief Initialises all pins that are used by the LED module.
 */
void LED_init(void);

/**
 * @brief Creates a tact by setting the clock flag value to LOW, HIGH, Low
 */
void LED_clock(void);

/**
 * @brief Updates the state of led by shifting the registry.
 * @details Uses #LED_clock to shift the registry
 * @param state Defines the set led's
 * @sa #LED_State
 */
void LED_set(LED_State state);

#endif
