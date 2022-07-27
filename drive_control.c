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
    //timers_setup_timer_0();

    // Set PB0, PB1, and PB3 as output (IN[2|3|4])
    //DDRB |= (1 << DD0) | (1 << DD1) | (1 << DD3);
    DR_M_LB |= (1 << DP_M_LB);
    DR_M_RB |= (1 << DP_M_RB);
    DR_M_RF |= (1 << DP_M_RF);
    //motor_set_speed(SPEED_STRAIT, SPEED_STRAIT);

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
    if (dir == OR_FORWARDS) {
        OR_M_RF |= (1 << OP_M_RF); // Forward ON
        OR_M_RB &= ~(1 << OP_M_RB); // Backward OFF
    } else if (dir == OR_BACKWARDS) {
        OR_M_RF &= ~(1 << OP_M_RF); // Forward OFF
        OR_M_RB |= (1 << OP_M_RB); // Backward ON
    } else {
        OR_M_RF &= ~(1 << OP_M_RF); // Forward OFF
        OR_M_RB &= ~(1 << OP_M_RB); // Backward OFF
        setDutyCycle(DP_M_LE, 0);
        return;
    }
    setDutyCycle(DP_M_RE, speed_state);
}

void motor_set_left(orientation dir, speed_value speed_state) {
    if (dir == OR_FORWARDS) {
        OR_M_LF |= (1 << OP_M_LF); // Forward ON
        OR_M_LB &= ~(1 << OP_M_LB); // Backward OFF
    } else if (dir == OR_BACKWARDS) {
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
    motor_set_speed(SPEED_INNER, SPEED_OUTER);
//    OR_M_LF |= (1 << OP_M_LF); // Left Forward ON
//    OR_M_RB |= (1 << OP_M_RB); // Right Backward ON
//    OR_M_LB &= ~(1 << OP_M_LB); // Left Backward OFF
//    OR_M_RF &= ~(1 << OP_M_RF); // Right Forward OFF
    // PORTD |= (1 << PD7);
    // PORTB |= (1 << PB1);
    // PORTB &= ~(1 << PB0);
    // PORTB &= ~(1 << PB3);
    motor_set_left(OR_FORWARDS, SPEED_INNER);
    motor_set_right(OR_BACKWARDS, SPEED_OUTER);
}

void motor_drive_forward(void) {
//    motor_set_speed(SPEED_STRAIT, SPEED_STRAIT);
    //PORTD |= (1 << PD7);
    //PORTB |= (1 << PB3);
    //PORTB &= ~(1 << PB0);
    //PORTB &= ~(1 << PB1);
//    OR_M_LF |= (1 << OP_M_LF); //Left Forward ON
//    OR_M_RF |= (1 << OP_M_RF); //Right Forward ON
//    OR_M_LB &= ~(1 << OP_M_LB); //Left Backward OFF
//    OR_M_RB &= ~(1 << OP_M_RB); //Right Backward OFF
    motor_set_left(OR_FORWARDS, SPEED_STRAIT);
    motor_set_right(OR_FORWARDS, SPEED_STRAIT);
}

void motor_drive_backward(void) {
//    motor_set_speed(SPEED_STRAIT, SPEED_STRAIT);
//    OR_M_LF &= ~(1 << OP_M_LF); //Left Forward ON
//    OR_M_RF &= ~(1 << OP_M_RF); //Right Forward ON
//    OR_M_LB |= (1 << OP_M_LB); //Left Backward OFF
//    OR_M_RB |= (1 << OP_M_RB); //Right Backward OFF
    motor_set_left(OR_BACKWARDS, SPEED_STRAIT);
    motor_set_right(OR_BACKWARDS, SPEED_STRAIT);
}

void motor_drive_left(void) {
//    motor_set_speed(SPEED_OUTER, SPEED_INNER);
//    //PORTB |= (1 << PB0); //Left Backward
//    //PORTB |= (1 << PB3); //Right Forward
//    //PORTB &= ~(1 << PB1);
//    //PORTD &= ~(1 << PD7);
//    OR_M_LB |= (1 << OP_M_LB); // Left Backward ON
//    OR_M_RF |= (1 << OP_M_RF); // Right Forward ON
//    OR_M_LF &= ~(1 << OP_M_LF); // Left Forward OFF
//    OR_M_RB &= ~(1 << OP_M_RB); // Right Backward OFF
    motor_set_right(OR_FORWARDS, SPEED_INNER);
    motor_set_left(OR_BACKWARDS, SPEED_OUTER);
}

void motor_drive_stop(void) {
    motor_set_speed(SPEED_ZERO, SPEED_ZERO);
    //Reset
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);
    PORTB &= ~(1 << PB3);
    PORTD &= ~(1 << PD7);
}

direction evaluate_sensors(sensor_state current, sensor_state last) {
    if(current == SENSOR_NONE){
        // Right Sensor
        if ((last & SENSOR_RIGHT)) {
            return DIR_RIGHT;
        }
            // Left Sensor
        else if ((last & SENSOR_LEFT)) {
            return DIR_LEFT;
        }
    }
    if ((current & SENSOR_CENTER) && ((current & SENSOR_LEFT) && (current & SENSOR_RIGHT) ||
                                           !(current & SENSOR_LEFT) && !(current & SENSOR_RIGHT))) {
        return DIR_FORWARD;
    }
    return DIR_NONE;
}

void drive_apply(sensor_state current, sensor_state last) {
    direction dir = evaluate_sensors(current, last);
    switch (dir) {
        case DIR_FORWARD:
            motor_drive_forward();
            break;
        case DIR_RIGHT:
            motor_drive_right();
            break;
        case DIR_LEFT:
            motor_drive_left();
            break;
        default:
            break;
    }
}

void drive_run(track_state *state) {
    switch (state->drive) {
        case DS_CHECK_START:
            //When on start field begin first round
            if (state->pos == POS_START_FIELD) {
                state->drive = DS_ZERO_ROUND;
                USART_print("Here I go again on my own, going down the only round I’ve ever known…\n");
            }
            break;
        case DS_ZERO_ROUND:
        case DS_FIRST_ROUND:
        case DS_SECOND_ROUND:
        case DS_THIRD_ROUND:
            if (timers_check_state(state, COUNTER_12_HZ)) {
                // Check if we were on track before and are now on the start field, WE DID A ROUND
                if (state->pos == POS_TRACK && state->last_pos == POS_START_FIELD) {
                    switch (state->drive) {
                        case DS_ZERO_ROUND:
                            state->drive = DS_FIRST_ROUND;
                            break;
                        case DS_FIRST_ROUND:
                            USART_print("YEAH, done round 1, going for round 2/3\n");
                            state->drive = DS_SECOND_ROUND;
                            break;
                        case DS_SECOND_ROUND:
                            USART_print("YEAH YEAH, done round 2, going for round 3/3\n");
                            state->drive = DS_THIRD_ROUND;
                            break;
                        case DS_THIRD_ROUND:
                            USART_print("YEAH YEAH YEAH , I really did it my way. ... And what's my purpose\n"
                                        "and the general sense of my further life now? Type ? for help\n");
                            state->drive = DS_BACKWARDS;
                            break;
                        default:
                            //Should never happen
                            break;
                    }
                }
            }
            drive_apply(state->sensor_current, state->sensor_last);
            break;
        case DS_BACKWARDS:
            motor_drive_backward();
            if(state->sensor_current == SENSOR_ALL){
                state->drive = DS_POST_DRIVE;
            }
            break;
        case DS_POST_DRIVE:
            motor_drive_stop();
            state->action = AC_RESET;
            break;
        case DS_PRE_DRIVE:
            break;
    }

    //driveDo(current);
}