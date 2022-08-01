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

void motor_update_brick(track_state *state, sensor_state current){
    state->sensor_brick <<= 3;
    state->sensor_brick |= current;
    state->sensor_brick &= BRICK_ALL;
}

void print_bits(uint16_t brick){
    int i = 0;
    char s[2];
    for(;i<BRICK_CACHED_AMOUNT * 3;++i){
        sprintf(s, "%d", (brick & BRICK_ALL) ? 1 : 0);
        // print last bit and shift left.
        usart_print(s);
        brick <<=1;
    }
    usart_print("\n");
}

sensor_state motor_brick_sensor(uint16_t brick, sensor_state state, uint8_t threshold){
    int v = 0;
    for(int i = 0;i < BRICK_CACHED_AMOUNT;i++){
        if(brick & (state << i)){
            v++;
        }
    }
    return v >= threshold ? state : SENSOR_NONE;
}

sensor_state motor_evaluate_brick(uint16_t brick){
    sensor_state brick_state = SENSOR_NONE;
    brick_state |= motor_brick_sensor(brick, SENSOR_LEFT, BRICK_THRESHOLD);
    brick_state |= motor_brick_sensor(brick, SENSOR_RIGHT, BRICK_THRESHOLD);
    brick_state |= motor_brick_sensor(brick, SENSOR_CENTER, BRICK_THRESHOLD);
    return brick_state;
}

//direction motor_evaluate_sensors(sensor_state current, sensor_state last) {
//    // None
//    /*if(!current){
//        if(last & SENSOR_LEFT) {
//            return DIR_LEFT;
//        }else if (last & SENSOR_RIGHT) {
//            return DIR_RIGHT;
//        }
//    }*/
//    if(current & SENSOR_LEFT && !(current & SENSOR_RIGHT)) {
//        return DIR_LEFT;
//    }else if (current & SENSOR_RIGHT && !(current & SENSOR_LEFT)) {
//        return DIR_RIGHT;
//    }else if ((current & SENSOR_CENTER)) {
//        return DIR_FORWARD;
//    }
//    return DIR_NONE;
//}

direction motor_evaluate_sensors(sensor_state current, sensor_state last) {
    if (current == SENSOR_NONE) {
        // Right Sensor
        if (last & SENSOR_RIGHT) {
            return DIR_RIGHT;
        }
        // Left Sensor
        else if (last & SENSOR_LEFT) {
            return DIR_LEFT;
        }
    }
    if ((current & SENSOR_CENTER) && ((current & SENSOR_LEFT) && (current & SENSOR_RIGHT) ||
                                      !(current & SENSOR_LEFT) && !(current & SENSOR_RIGHT))) {
        return DIR_FORWARD;
    }
    return DIR_NONE;
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
    state->last_dir = dir;
}

sensor_state last_valid;
sensor_state last_;

void drive_apply(track_state *state, sensor_state current, sensor_state last) {
    sensor_state brick_last = motor_evaluate_brick(state->sensor_brick);
    motor_update_brick(state, current);
    sensor_state brick_current = motor_evaluate_brick(state->sensor_brick);
//    print_bits(0b000011001111);
//    direction dir = motor_evaluate_sensors(current, last);
    if(current && current != last_valid){
        last_valid = brick_current;
    }
    if(last_ != current) {
        char s[sizeof("00\n")];
        sprintf(s, "%d\n", current);
        usart_print(s);
        last_ = current;
    }
    direction dir = motor_evaluate_sensors(current, last);
    drive_move_direction(state, dir);
}

void drive_home(track_state *state) {
    switch (state->drive) {
        case DS_ZERO_ROUND: //Fallthrough
        case DS_FIRST_ROUND: //Fallthrough
        case DS_SECOND_ROUND: //Fallthrough
        case DS_THIRD_ROUND: //Fallthrough
            //When on start field begin first round
            if (state->pos == POS_START_FIELD) {
                state->drive = DS_BACKWARDS;
            }
            break;
        case DS_BACKWARDS:
            drive_move_direction(state, DIR_BACK);
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
    if(timers_check_state(state, COUNTER_1_HZ) &&( state->manual_dir || state->manual_dir_last)) {
        if(state->manual_dir_last){
            motor_drive_stop();
            state->manual_dir_last = 0;
            usart_print("drive stop\n");
        }else {
            drive_move_direction(state, state->manual_dir);
            state->manual_dir = DIR_NONE;
            state->manual_dir_last = 1;
            usart_print("drive\n");
        }
    }
}

void drive_run(track_state *state) {
    switch (state->drive) {
        case DS_CHECK_START:
            //When on start field begin first round
            if (state->pos == POS_START_FIELD) {
                state->drive = DS_ZERO_ROUND;
                usart_print(
                        "Here I go again on my own, going down the only round I’ve ever known…"
                        "\n"
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
                        usart_print("YEAH, done round 1, going for round 2/3\n");
                        state->drive = DS_SECOND_ROUND;
                        break;
                    case DS_SECOND_ROUND:
                        usart_print("YEAH YEAH, done round 2, going for round 3/3\n");
                        state->drive = DS_THIRD_ROUND;
                        break;
                    case DS_THIRD_ROUND:
                        usart_print(
                                "YEAH YEAH YEAH , I really did it my way. ... And what's my "
                                "purpose\n and the general sense of my further life now?"
                                " Type ? for help\n");
                        state->drive = DS_BACKWARDS;
                        break;
                    default:
                        //Should never happen
                        break;
                }
            }
            drive_apply(state, state->sensor_current, state->sensor_last);
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