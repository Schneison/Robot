/**
 * @file robot_controller.c
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief controlls movmentof the roboter
 * 
 */
#ifndef RO_DRIVE
#define RO_DRIVE

//IN1 Left Forward
#define DR_M_LF DDRD
#define DP_M_LF DD7
#define OR_M_LF PORTD
#define OP_M_LF PD7

//IN2 Left Backward
#define DR_M_LB DDRB
#define DP_M_LB DD0
#define OR_M_LB PORTB
#define OP_M_LB PB0

//IN4 Right Forward
#define DR_M_RF DDRB
#define DP_M_RF DD3
#define OR_M_RF PORTB
#define OP_M_RF PB3

//IN3 Right Backward
#define DR_M_RB DDRB
#define DP_M_RB DD1
#define OR_M_RB PORTB
#define OP_M_RB PB1

// Left Enable
#define DR_M_LE DDRD
#define DP_M_LE DD5
#define OR_M_LE PORTD
#define OP_M_LE PD5

// Right Enable Motor
#define DR_M_RE DDRD
#define DP_M_RE DD6
#define OR_M_RE PORTD
#define OP_M_RE PD6

void clear(void);

/**
 * @brief Sets the speed of the two motors.
 * 
 * @param left_speed Speed state of the left motor.
 * @param right_speed Speed state of the left motor.
 * 
 */
void set_speed(uint8_t left_speed, uint8_t right_speed);

/**
 * @brief Sets the speed of the two motors.
 * 
 */
void drive_right(void);

void drive_forward(void);

void drive_backward(void);

void drive_left(void);

#endif
