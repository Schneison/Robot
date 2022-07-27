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
/**
* @brief Signals that this is a chase state and we are on the way back
*/
    CHASE_FLAG = 8,
} LED_State;

/**
 * @brief Describes the binary state of the sensors
 */
typedef enum {
    /**
 * @brief No sensor signal
 */
    SENSOR_NONE = 0,
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
 * @brief Initialises all pins that are used by the LED module.
 */
void led_init(void);

/**
 * @brief Creates a tact by setting the clock flag value to LOW, HIGH, Low
 */
void led_clock(void);

/**
 * @brief Updates the state of led by shifting the registry.
 * @details Uses #led_clock to shift the registry
 * @param state Defines the set led's
 * @sa #LED_State
 */
void led_set(LED_State state);

/**
 * Updates the chase light that is used in some action states.
 *
 * @param lastState The last state of the chase light.
 */
void led_chase(LED_State *lastState);

/**
 * Enables all led's if they are off, and vise versa.
 *
 * @param lastState The last state of the blink light.
 */
void led_blink(LED_State *lastState);

/**
 * Lets the led's show the current state of the sensors.
 *
 * @param sensorState The current state of the field sensors.
 */
void led_sensor(sensor_state sensorState);

#endif
