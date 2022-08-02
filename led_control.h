/**
 * @file
 * @author Larson Schneider
 * @date 09.07.2022
 * @brief Contains functions and structs for handling the leds (light-emitting diodes) of the robot
 * @version 0.1
 *
 * This module is mainly used to control the leds.
 *
 * Most importantly are the functions that are called to handle states from other modules and
 * convert these to an led state. @n
 * For reference see @ref led_chase, @ref led_blink, @ref led_sensor
 */
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <avr/io.h>

// SR clock
/** @brief Direction Register of the shift clock */
#define DR_SR_CLK  DDRD
/** @brief Direction Register Pin of shift clock   */
#define DP_SR_CLK  DDD4

// SR data
/** @brief Direction Register of data to shift */
#define DR_SR_DATA DDRB
/** @brief Direction Register Pin of data to shift  */
#define DP_SR_DATA DDB2

/** @brief LED shift register port */
#define LED_PORT PORTB
/** @brief LED shift register input pin */
#define LED_INPUT PB2
/** @brief LED shift register clock flag pin */
#define LED_CLOCK PD4
/** @brief LED shift register clock port */
#define LED_CLOCK_PORT PORTD
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
} led_state;

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
 * @brief Initialises all pins and registries that are used by the LED module.
 *
 *
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
 * @sa #led_state
 */
void led_set(led_state state);

/**
 * Updates the chase light that is used in some action states.
 *
 * @param lastState The last state of the chase light.
 */
void led_chase(led_state *lastState);

/**
 * Enables all led's if they are off, and vise versa.
 *
 * @param lastState The last state of the blink light.
 */
void led_blink(led_state *lastState);

/**
 * Lets the led's show the current state of the sensors.
 *
 * @param sensorState The current state of the field sensors.
 */
void led_sensor(sensor_state sensorState);

#endif
