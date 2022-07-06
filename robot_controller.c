#include "robot_controller.h"

//IN1 Left Forward
//IN2 Left Backward
//IN4 Right Forward
//IN3 Right Backward

void motor_clear(void) {
	// Delete everything on ports B and D
    DDRD = 0;
    DDRB = 0;
}

void motor_init(void) {
    // Set PD5 and PD6 as output (EN[A|B]!)
    //DDRD |= (1 << DD5) | (1 << DD6);
    DR_M_LE |= (1 << DP_M_LE);
    DR_M_RE |= (1 << DP_M_RE);
    
    // Set PD7 as output (IN1)
    //DDRD |= (1 << DD7);
    DR_M_LF |= (1 << DP_M_LF);

    // Make PWM work on PD[5|6]
    setupTimer0();

    // Set PB0, PB1, and PB3 as output (IN[2|3|4])
    // DDRB |= (1 << DD0) | (1 << DD1) | (1 << DD3);
    DR_M_LB |= (1 << DP_M_LB);
    DR_M_RB |= (1 << DP_M_RB);
    DR_M_RF |= (1 << DP_M_RF);
    
}

void set_speed(uint8_t left_speed, uint8_t right_speed) {
	setDutyCycle(DP_M_LE, left_speed == STATE_HIGH ? 190 : left_speed == STATE_MIDDLE ? 155 : 110); // left
	setDutyCycle(DP_M_RE, right_speed == STATE_HIGH ? 190 : right_speed == STATE_MIDDLE ? 155 : 110); // right
}
	
void drive_right(void) {
	set_speed(STATE_LOW, STATE_HIGH);
	OR_M_LF |= (1 << OP_M_LF); // Left Forward ON
	OR_M_RB |= (1 << OP_M_RB); // Right Backward ON
	OR_M_LB &= ~(1 << OP_M_LB); // Left Backward OFF
	OR_M_RF &= ~(1 << OP_M_RF); // Right Forward OFF
	// PORTD |= (1 << PD7); 
	// PORTB |= (1 << PB1); 
	// PORTB &= ~(1 << PB0);
	// PORTB &= ~(1 << PB3);
}

void drive_forward(void) {
	set_speed(STATE_MIDDLE, STATE_MIDDLE);
	//PORTD |= (1 << PD7);
	//PORTB |= (1 << PB3);
	//PORTB &= ~(1 << PB0);
	//PORTB &= ~(1 << PB1);
	OR_M_LF |= (1 << OP_M_LF); //Left Forward ON
	OR_M_RF |= (1 << OP_M_RF); //Right Forward ON
	OR_M_LB &= ~(1 << OP_M_LB); //Left Backward OFF
	OR_M_RB &= ~(1 << OP_M_RB); //Right Backward OFF
}

void drive_backward(void) {
    set_speed(STATE_MIDDLE, STATE_MIDDLE);
    OR_M_LF &= ~(1 << OP_M_LF); //Left Forward ON
    OR_M_RF &= ~(1 << OP_M_RF); //Right Forward ON
    OR_M_LB |= (1 << OP_M_LB); //Left Backward OFF
    OR_M_RB |= (1 << OP_M_RB); //Right Backward OFF
}

void drive_left(void) {
	set_speed(STATE_HIGH, STATE_LOW);
	//PORTB |= (1 << PB0); //Left Backward
	//PORTB |= (1 << PB3); //Right Forward	
	//PORTB &= ~(1 << PB1);
	//PORTD &= ~(1 << PD7);
	OR_M_LB |= (1 << OP_M_LB); // Left Backward ON
	OR_M_RF |= (1 << OP_M_RF); // Right Forward ON
	OR_M_LF &= ~(1 << OP_M_LF); // Left Forward OFF
	OR_M_RB &= ~(1 << OP_M_RB); // Right Backward OFF	
}

void drive_clear(void) {
    //Reset
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);
    PORTB &= ~(1 << PB3);
    PORTD &= ~(1 << PD7);
}