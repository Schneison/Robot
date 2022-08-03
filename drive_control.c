#include "drive_control.h"

void motor_clear(void) {
    // Delete everything on ports B and D
    DR_MOTOR_FIRST = 0;
    DR_MOTOR_SECOND = 0;
}

void motor_init(void) {
    // Set PD5 and PD6 as output (EN[A|B]!)
    DR_M_LE |= (1 << DP_M_LE);
    DR_M_RE |= (1 << DP_M_RE);

    // Set PD7 as output (IN1)
    DR_M_LF |= (1 << DP_M_LF);

    // Set PB0, PB1, and PB3 as output (IN[2|3|4])
    DR_M_LB |= (1 << DP_M_LB);
    DR_M_RB |= (1 << DP_M_RB);
    DR_M_RF |= (1 << DP_M_RF);

}

void motor_set_duty(uint8_t pin, speed_value value) {
    if (pin == OP_M_RE) {
        if (value == 0) {
            TIMER_0_WAVE &= TIMER_0_NORMAL_OPERATION_A;  // Normal port operation mode
            OR_M_RE &= ~(1 << OP_M_RE);                       // PD6 LOW, equals 0% duty,
        }                                             // timer disconnected
        else if (value == 255) {
            TIMER_0_WAVE &= TIMER_0_NORMAL_OPERATION_A;  // Normal port operation mode
            OR_M_RE |= (1 << OP_M_RE);                        // PD6 HIGH, equals 100% duty,
        }                                             // timer disconnected
        else {
            TIMER_0_WAVE |= (1 << COM0A1);                    // OC0A to LOW on Compare Match,
            TIMER_0_WAVE &=
                    ~(1 << COM0A0);                   // to HIGH at BOTTOM (non-inverting mode)
            TIMER_0_COMPARE_RESOLUTION_A = (uint8_t) value;                              // generates sequences of 1-0-1-0...
        }                                             // for certain periods of time
    }

    if (pin == OP_M_LE) {
        if (value == 0) {
            TIMER_0_WAVE &= TIMER_0_NORMAL_OPERATION_B;
            OR_M_LE &= ~(1 << OP_M_LE);
        } else if (value == 255) {
            TIMER_0_WAVE &= TIMER_0_NORMAL_OPERATION_B;
            OR_M_LE |= (1 << OP_M_LE);
        } else {
            TIMER_0_WAVE |= (1 << COM0B1);
            TIMER_0_WAVE &= ~(1 << COM0B0);
            TIMER_0_COMPARE_RESOLUTION_B = (uint8_t) value;
        }
    }
}

void motor_set_speed(speed_value left_speed, speed_value right_speed) {
    motor_set_duty(DP_M_LE, left_speed); // left
    motor_set_duty(DP_M_RE, right_speed); // right
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
        motor_set_duty(DP_M_LE, 0);
        return;
    }
    motor_set_duty(DP_M_RE, speed_state);
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
        motor_set_duty(DP_M_LE, 0);
        return;
    }
    motor_set_duty(DP_M_LE, speed_state);
}

void motor_drive_right(void) {
    motor_set_left(OR_FORWARDS, SPEED_OUTER);
    motor_set_right(OR_BACKWARDS, SPEED_INNER);
}

void motor_drive_forward(void) {
    motor_set_left(OR_FORWARDS, SPEED_STRAIT);
    motor_set_right(OR_FORWARDS, SPEED_STRAIT);
}

void motor_drive_backward(void) {
    motor_set_left(OR_BACKWARDS, SPEED_STRAIT);
    motor_set_right(OR_BACKWARDS, SPEED_STRAIT);
}

void motor_drive_left(void) {
    motor_set_right(OR_FORWARDS, SPEED_OUTER);
    motor_set_left(OR_BACKWARDS, SPEED_INNER);
}

void motor_drive_stop(void) {
    motor_set_left(OR_STOP, SPEED_ZERO);
    motor_set_right(OR_STOP, SPEED_ZERO);
}

direction motor_evaluate_sensors(sensor_state current) {
    if ((current & SENSOR_CENTER)
        && ((current & SENSOR_LEFT) == (current & SENSOR_RIGHT))
        || !(current & SENSOR_LEFT) == !(current & SENSOR_RIGHT)) {
        return DIR_FORWARD;
    }
    if (current & SENSOR_RIGHT) {
        return DIR_RIGHT;
    }
    if (current & SENSOR_LEFT) {
        return DIR_LEFT;
    }
    return DIR_NONE;
}

direction motor_calc_direction(
        sensor_state current,
        sensor_state last_state,
        direction *last_dir,
        direction *last_simple
       ) {
    if(current == SENSOR_NONE) {
        if(last_state == SENSOR_ALL){
            usart_print_pretty("Used last simple state");
            char s[sizeof("%d")];
            sprintf(s, "%d", *last_simple);
            usart_println(s);
            return *last_simple;
        }
        return *last_dir;
    }else{
        *last_dir = motor_evaluate_sensors(current);
        if(*last_dir == SENSOR_RIGHT || *last_dir == SENSOR_LEFT){
            *last_simple = *last_dir;
        }
        return *last_dir;
    }
}

void drive_move_direction(track_state *state, direction dir) {
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
        case DIR_BACK:
            motor_drive_backward();
            break;
        default:
            break;
    }
    state->dir_last = dir;
}

sensor_state last_s;
direction last_;

void drive_apply(track_state *state) {
    direction dir = motor_calc_direction(state->sensor_current,
                                         SENSOR_RIGHT,
                                         &(state->dir_last_valid),
                                         &(state->dir_last_simple));
    if(last_s != state->sensor_current) {
        char s[sizeof("S: %d")];
        sprintf(s, "S: %d", state->sensor_current);
        usart_println(s);
        last_s = state->sensor_current;
    }
    if(last_ != dir) {
        char s[sizeof("D: %d")];
        sprintf(s, "D: %d", dir);
        usart_println(s);
        last_ = dir;
    }
    drive_move_direction(state, dir);
}

void drive_home(track_state *state) {
    switch (state->drive) {
        case DS_ZERO_ROUND: //Fallthrough
        case DS_FIRST_ROUND: //Fallthrough
        case DS_SECOND_ROUND: //Fallthrough
        case DS_THIRD_ROUND: //Fallthrough
            //When on start field stop
            if (state->last_pos == POS_START_FIELD) {
                state->drive = DS_BACKWARDS;
            }
            drive_apply(state);
            break;
        case DS_BACKWARDS:
            drive_move_direction(state, DIR_BACK);
            // Stop if on starting field
            if (state->sensor_current == SENSOR_ALL) {
                state->drive = DS_POST_DRIVE;
            }
            break;
        case DS_CHECK_START:
        case DS_POST_DRIVE:
            motor_drive_stop();
            state->action = AC_RESET;
            break;
        case DS_PRE_DRIVE:
            break;
    }
}

void drive_manual(track_state *state) {
    if(timers_check_state(state, COUNTER_1_HZ)
        && ( state->manual_dir || state->manual_dir_last)) {
        if(state->manual_dir_last){
            motor_drive_stop();
            state->manual_dir_last = 0;
        } else {
            drive_move_direction(state, state->manual_dir);
            state->manual_dir = DIR_NONE;
            state->manual_dir_last = 1;
        }
    }
}

void drive_run(track_state *state) {
    switch (state->drive) {
        case DS_CHECK_START:
            //When on start field begin first round
            if (state->pos == POS_START_FIELD) {
                state->drive = DS_ZERO_ROUND;
                usart_println(
                        "Here I go again on my own, going down the only round I’ve ever known…"
                );
            }
            break;
        case DS_ZERO_ROUND: //Fallthrough
        case DS_FIRST_ROUND: //Fallthrough
        case DS_SECOND_ROUND: //Fallthrough
        case DS_THIRD_ROUND: //Fallthrough
            if (timers_check_state(state, COUNTER_12_HZ) &&
                state->pos == POS_TRACK && state->last_pos == POS_START_FIELD) {
                switch (state->drive) {
                    case DS_ZERO_ROUND:
                        state->drive = DS_FIRST_ROUND;
                        break;
                    case DS_FIRST_ROUND:
                        usart_print_pretty("YEAH, done round 1, going for round 2/3");
                        state->drive = DS_SECOND_ROUND;
                        break;
                    case DS_SECOND_ROUND:
                        usart_print_pretty("YEAH YEAH, done round 2, going for round 3/3");
                        state->drive = DS_THIRD_ROUND;
                        break;
                    case DS_THIRD_ROUND:
                        usart_print_pretty(
                                "YEAH YEAH YEAH , I really did it my way. ... And what's my "
                                "purpose\n and the general sense of my further life now?"
                                " Type ? for help");
                        state->drive = DS_BACKWARDS;
                        break;
                    default:
                        //Should never happen
                        break;
                }
            }
            drive_apply(state);
            break;
        case DS_BACKWARDS:
            motor_drive_backward();
            if (state->sensor_current == SENSOR_ALL) {
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
}