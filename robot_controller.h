/**
 * @file robot_controller.h
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Controls movement of the roboter
 * @version 0.1
 */
#ifndef RO_DRIVE
#define RO_DRIVE

#include <avr/io.h>
#include "iesmotors.h"
#include <util/delay.h>
#include "robot_sensor.h"
#include "iesusart.h"

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
 * @brief Clears all registers that the drive module uses
 */
void motor_clear(void);

/**
 * @brief Initialises the drive module
 */
void motor_init(void);

/**
 * @brief Sets the speed of the two motors.
 *
 * @param left_speed Speed state of the left motor.
 * @param right_speed Speed state of the left motor.
 *
 */
void motor_set_speed(uint8_t left_speed, uint8_t right_speed);

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
 * @brief Clears all motor register pins
 */
void motor_clear_drive(void);

#endif
