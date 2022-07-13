#include "drive_control.h"

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
    setupMotorTimer();

    // Set PB0, PB1, and PB3 as output (IN[2|3|4])
    //DDRB |= (1 << DD0) | (1 << DD1) | (1 << DD3);
    DR_M_LB |= (1 << DP_M_LB);
    DR_M_RB |= (1 << DP_M_RB);
    DR_M_RF |= (1 << DP_M_RF);

}

void setDutyCycle(uint8_t pin, uint8_t value) {
    // Suggestion to handle PD6 - note the code-clones wrt. PD5 below!
    // Code-clones are extraordinary f cky! Correct this (tricky though
    // due to the PP-macros, which you cannot simply pass to functions)!
    // (But PP-macros can help you here....)
    if (pin == PD6) {
        if (value == 0) {
            TCCR0A &= ~(1 << COM0A1) & ~(1 << COM0A0);  // Normal port operation mode
            PORTD &= ~(1 << PD6);                       // PD6 LOW, equals 0% duty,
        }                                             // timer disconnected
        else if (value == 255) {
            TCCR0A &= ~(1 << COM0A1) & ~(1 << COM0A0);  // Normal port operation mode
            PORTD |= (1 << PD6);                        // PD6 HIGH, equals 100% duty,
        }                                             // timer disconnected
        else {
            TCCR0A |= (1 << COM0A1);                    // OC0A to LOW on Compare Match,
            TCCR0A &= ~(1 << COM0A0);                   // to HIGH at BOTTOM (non-inverting mode)
            OCR0A = value;                              // generates sequences of 1-0-1-0...
        }                                             // for certain periods of time
    }

    if (pin == PD5) {
        if (value == 0) {
            TCCR0A &= ~(1 << COM0B1) & ~(1 << COM0B0);
            PORTD &= ~(1 << PD5);
        } else if (value == 255) {
            TCCR0A &= ~(1 << COM0B1) & ~(1 << COM0B0);
            PORTD |= (1 << PD5);
        } else {
            TCCR0A |= (1 << COM0B1);
            TCCR0A &= ~(1 << COM0B0);
            OCR0B = value;
        }
    }
}

void motor_set_speed(speed_value left_speed, speed_value right_speed) {
    setDutyCycle(DP_M_LE, left_speed); // left
    setDutyCycle(DP_M_RE, right_speed); // right
}

void motor_set_right(orientation dir, speed_value speed_state) {
    if (dir == FORWARDS) {
        OR_M_LF |= (1 << OP_M_RF); // Forward ON
        OR_M_LB &= ~(1 << OP_M_RB); // Backward OFF
    } else if (dir == BACKWARDS) {
        OR_M_LF &= ~(1 << OP_M_RF); // Forward OFF
        OR_M_LB |= (1 << OP_M_RB); // Backward ON
    } else {
        OR_M_LF &= ~(1 << OP_M_RF); // Forward OFF
        OR_M_LB &= ~(1 << OP_M_RB); // Backward OFF
        setDutyCycle(DP_M_LE, 0);
        return;
    }
    setDutyCycle(DP_M_RE, speed_state);
}

void motor_set_left(orientation dir, speed_value speed_state) {
    if (dir == FORWARDS) {
        OR_M_LF |= (1 << OP_M_LF); // Forward ON
        OR_M_LB &= ~(1 << OP_M_LB); // Backward OFF
    } else if (dir == BACKWARDS) {
        OR_M_LF &= ~(1 << OP_M_LF); // Forward OFF
        OR_M_LB |= (1 << OP_M_LB); // Backward ON
    } else {
        OR_M_LF &= ~(1 << OP_M_LF); // Forward OFF
        OR_M_LB &= ~(1 << OP_M_LB); // Backward OFF
        setDutyCycle(DP_M_LE, 0);
        return;
    }
    setDutyCycle(DP_M_LE, speed_state);
}

void motor_drive_right(void) {
    motor_set_speed(SPEED_LOW, SPEED_HIGH);
    OR_M_LF |= (1 << OP_M_LF); // Left Forward ON
    OR_M_RB |= (1 << OP_M_RB); // Right Backward ON
    OR_M_LB &= ~(1 << OP_M_LB); // Left Backward OFF
    OR_M_RF &= ~(1 << OP_M_RF); // Right Forward OFF
    // PORTD |= (1 << PD7);
    // PORTB |= (1 << PB1);
    // PORTB &= ~(1 << PB0);
    // PORTB &= ~(1 << PB3);
}

void motor_drive_forward(void) {
    motor_set_speed(SPEED_MIDDLE, SPEED_MIDDLE);
    //PORTD |= (1 << PD7);
    //PORTB |= (1 << PB3);
    //PORTB &= ~(1 << PB0);
    //PORTB &= ~(1 << PB1);
    OR_M_LF |= (1 << OP_M_LF); //Left Forward ON
    OR_M_RF |= (1 << OP_M_RF); //Right Forward ON
    OR_M_LB &= ~(1 << OP_M_LB); //Left Backward OFF
    OR_M_RB &= ~(1 << OP_M_RB); //Right Backward OFF
}

void motor_drive_backward(void) {
    motor_set_speed(SPEED_MIDDLE, SPEED_MIDDLE);
    OR_M_LF &= ~(1 << OP_M_LF); //Left Forward ON
    OR_M_RF &= ~(1 << OP_M_RF); //Right Forward ON
    OR_M_LB |= (1 << OP_M_LB); //Left Backward OFF
    OR_M_RB |= (1 << OP_M_RB); //Right Backward OFF
}

void motor_drive_left(void) {
    motor_set_speed(SPEED_HIGH, SPEED_LOW);
    //PORTB |= (1 << PB0); //Left Backward
    //PORTB |= (1 << PB3); //Right Forward
    //PORTB &= ~(1 << PB1);
    //PORTD &= ~(1 << PD7);
    OR_M_LB |= (1 << OP_M_LB); // Left Backward ON
    OR_M_RF |= (1 << OP_M_RF); // Right Forward ON
    OR_M_LF &= ~(1 << OP_M_LF); // Left Forward OFF
    OR_M_RB &= ~(1 << OP_M_RB); // Right Backward OFF
}

void motor_clear_drive(void) {
    motor_set_speed(SPEED_ZERO, SPEED_ZERO);
    //Reset
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);
    PORTB &= ~(1 << PB3);
    PORTD &= ~(1 << PD7);
}