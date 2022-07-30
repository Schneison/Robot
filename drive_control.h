/**
 * @file drive_control.h
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Controls movement of the roboter
 * @version 0.1
 */
#ifndef MOTOR_DRIVE
#define MOTOR_DRIVE

#include <avr/io.h>
#include "timers.h"
#include <util/delay.h>
#include "robot_sensor.h"
#include "usart.h"
#include "utility.h"

// Direction Register = DR
// Input Register = IR
// Output Register = OR

//IN1 Left Forward
/** @brief Direction Register of the left wheel forward */
#define DR_M_LF DDRD
/** @brief Direction Pin of the left wheel forward */
#define DP_M_LF DD7
/** @brief Output Register of the left wheel forward */
#define OR_M_LF PORTD
/** @brief Output Pin of the left wheel forward */
#define OP_M_LF PD7

//IN2 Left Backward
/** @brief Direction Register of the left wheel backward */
#define DR_M_LB DDRB
/** @brief Direction Pin of the left wheel backward */
#define DP_M_LB DD0
/** @brief Output Register of the left wheel backward */
#define OR_M_LB PORTB
/** @brief Output Pin of the left wheel backward */
#define OP_M_LB PB0

//IN4 Right Forward
/** @brief Direction Register of the right wheel forward */
#define DR_M_RF DDRB
/** @brief Direction Pin of the right wheel forward */
#define DP_M_RF DD3
/** @brief Output Register of the right wheel forward */
#define OR_M_RF PORTB
/** @brief Output Pin of the right wheel forward */
#define OP_M_RF PB3

//IN3 Right Backward
/** @brief Direction Register of the right wheel backward */
#define DR_M_RB DDRB
/** @brief Direction Pin of the right wheel backward */
#define DP_M_RB DD1
/** @brief Output Register of the right wheel backward */
#define OR_M_RB PORTB
/** @brief Output Pin of the right wheel backward */
#define OP_M_RB PB1

/** @brief First of the two data direction registries used by this module*/
#define DR_MOTOR_FIRST DDRD
/** @brief Second of the two data direction registries used by this module*/
#define DR_MOTOR_SECOND DDRB

// Left Enable
/** @brief Direction Register of the left motor speed */
#define DR_M_LE DDRD
/** @brief Direction Pin of the left motor speed */
#define DP_M_LE DD5
/** @brief Output Register of the left motor speed */
#define OR_M_LE PORTD
/** @brief Output Pin of the left motor speed  */
#define OP_M_LE PD5

// Right Enable Motor
/** @brief Direction Register of the right motor speed */
#define DR_M_RE DDRD
/** @brief Direction Pin of the right motor speed */
#define DP_M_RE DD6
/** @brief Output Register of the right motor speed */
#define OR_M_RE PORTD
/** @brief Output Pin of the right motor speed  */
#define OP_M_RE PD6


/**
 * @brief Possible directions of the two motors.
 */
typedef enum {
/**
* @brief Move motor forward
*/
    OR_FORWARDS,
/**
 * @brief Move motor backward
 */
    OR_BACKWARDS,
/**
 * @brief Stop the motor movement
 */
    OR_STOP
} orientation;

/**
 * @brief Defines the possible speed values of the motors.
 */
typedef enum {
/**
* @brief Stop the engine, no motion
*/
    SPEED_ZERO = 0,
/**
* @brief Speed of the inner wheel
*/
    SPEED_INNER = 150,
/**
* @brief Speed on a strait line
*/
    SPEED_STRAIT = 140,
/**
* @brief Speed of the outer wheel
*/
    SPEED_OUTER = 220
} speed_value;

/**
 * @brief Clears all registers that the drive module uses
 */
void motor_clear(void);

/**
 * @brief Initialises the drive module
 */
void motor_init(void);

/**
 * @brief Defines speed of the wheels by defining how often a they are turned on,
 * a higher value results in an faster wheel.
 *
 * @details Call #timers_setup_timer_0() before usage!
 * @param pin PD5/PD6 for left/right motor cycle
 * @param value Defines speed of the wheels. (0 - 255 = 0% - 100%)
 */
void motor_set_duty(uint8_t pin, speed_value value);

/**
 * @brief Sets the speed of the left motor.
 *
 * @param dir Direction of the motor motion
 * @param speed_state Speed of the motor.
 *
 */
void motor_set_left(orientation dir, speed_value speed_state);

/**
 * @brief Sets the speed of the right motor.
 *
 * @param dir Direction of the motor motion
 * @param speed_state Speed of the motor.
 *
 */
void motor_set_right(orientation dir, speed_value speed_state);

/**
 * @brief Sets the speed of the two motors.
 *
 * @param left_speed Speed state of the left motor.
 * @param right_speed Speed state of the righht motor.
 *
 */
void motor_set_speed(speed_value left_speed, speed_value right_speed);

/**
 * @brief Sets the values to drive the robot to the left
 */
void motor_drive_left(void);

/**
 * @brief Sets the values to drive the robot to the right
 */
void motor_drive_right(void);

/**
 * @brief Sets the values to drive the robot to the forward
 */
void motor_drive_forward(void);

/**
 * @brief Sets the values to drive the robot to the backward
 */
void motor_drive_backward(void);

/**
 * @brief Stops all motors and disables the duty cycle of the robot
 *
 * @details Sets all pins and the duty cycle to 0
 */
void motor_drive_stop(void);

/**
 * @brief Reads sensor input and evaluates the direction that the robot has to drive.
 * @param current Current sensor state
 * @param last Sensor state in the last cycle
 */
direction evaluate_sensors(sensor_state current, sensor_state last);

/**
 * @brief Perform driving of the robot
 *
 * @param current Current sensor state
 * @param last State of the sensors in the last cycle.
 */
void drive_apply(track_state *state, sensor_state current, sensor_state last);

/**
 * @brief Drive the robot into the given direction
 * @details Called by manual mode and by the line following after evaluation of the sensors
 *
 * @param state Current state
 * @param dir Direction to drive
 */
void drive_move_direction(track_state *state, direction dir);

/**
 * @brief Called home, finish this round and reset
 *
 * @param state Current state
 */
void drive_home(track_state *state);

/**
 * @brief Manual drive, controlled by the serial
 *
 * @param state Current state
 */
void drive_manual(track_state *state);

/**
 * @brief Performance the driving action
 *
 * @param state Current state
 */
void drive_run(track_state *state);

#endif
