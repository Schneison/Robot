#include "state_control.h"


void state_show(track_state *state) {
    switch (state->action) {
        case AC_ROUNDS: {
            uint8_t round = 1;
            switch (state->drive) {
                case DS_SECOND_ROUND:
                    round = 2;
                    break;
                case DS_THIRD_ROUND:
                    round = 3;
                    break;
                default:
                    break;
            }
            // Manuel check, so we don't have to create a pointer every tick
            if (timers_check_state(state, COUNTER_1_HZ)) {
                char s[sizeof("Round and round I go, currently round 1\n")];
                sprintf(s, "Round and round I go, currently round %d\n", round);
                USART_print(s);
            }
//            LED_State ledState = LED_NONE;
//            if ((state->sensor_last) & SENSOR_LEFT) {
//                ledState |= LED_LEFT;
//            }
//            if ((state->sensor_last) & SENSOR_CENTER) {
//                ledState |= LED_CENTER;
//            }
//            if ((state->sensor_last) & SENSOR_RIGHT) {
//                ledState |= LED_RIGHT;
//            }
//            LED_set(ledState);
            //TODO: Remove before deployment
            led_sensor(state->sensor_last);
            break;
        }
        case AC_FROZEN:
            timers_print(state->counters, COUNTER_1_HZ,
                         "In safe state! Won’t react to any instructions! Rescue me!\n");
            if (timers_check_state(state, COUNTER_12_HZ)) {
                led_chase(&(state->last_led));
            }
            break;
        case AC_RETURN_HOME:
            timers_print(state->counters, COUNTER_1_HZ, "Returning home, will util_reset me there\n");
            break;
        case AC_PAUSE:
            timers_print(state->counters, COUNTER_1_HZ,
                         "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            if (timers_check_state(state, COUNTER_3_HZ)) {
                led_chase(&(state->last_led));
            }
            break;
        case AC_WAIT:
            if (state->pos == POS_START_FIELD) {
                timers_print(state->counters, COUNTER_1_HZ,
                             "On the starting field. Waiting for your instructions... Send ? for help.\n");
                if (timers_check_state(state, COUNTER_5_HZ)) {
                    led_blink(&(state->last_led));
                }
            } else {
                timers_print(state->counters, COUNTER_1_HZ,
                             "Not on the starting field. Place me there please... Send ? for help.\n");
//                LED_State ledState = LED_NONE;
//                if ((state->sensor_last) & SENSOR_LEFT) {
//                    ledState |= LED_LEFT;
//                }
//                if ((state->sensor_last) & SENSOR_CENTER) {
//                    ledState |= LED_CENTER;
//                }
//                if ((state->sensor_last) & SENSOR_RIGHT) {
//                    ledState |= LED_RIGHT;
//                }
//                LED_set(ledState);
                led_sensor(state->sensor_last);
            }
            break;
        default:
            break;
    }
}

void state_print_help(track_state *state) {
    //Only print help text if S was not received once
    if (state->has_driven_once) {
        return;
    }
    if (state->pos == POS_START_FIELD) {
        USART_print("-X Safe action_state\n-S 3 Rounds\n-P Pause\n-R Reset\n-C Home\n-? Help\n");
    } else {
        USART_print("Not on start field, please position on start field!\n");
    }
}

void state_print_fail(unsigned char byte) {
    char s[90 * sizeof(char)];// More is better
    sprintf(s, "Received undefined Sign %c\n", byte);
    USART_print(s);
}

void state_on_action_change(track_state *state) {
    switch (state->action) {
        case AC_RESET:
            USART_print("Will util_reset in 5 seconds...\n");
            break;
        case AC_WAIT:
        case AC_FROZEN:
        case AC_PAUSE:
            //Reset chase or blink light
            state->last_led = 0;
            break;
        case AC_ROUNDS:
            state->has_driven_once = 1;
            break;
        default:
            break;
    }
}

void state_read_input(track_state *state) {
    if (!USART_canReceive()) {
        return;
    }
    if (state->action == AC_FROZEN) {
        return;
    }
    unsigned char byte = USART_receiveByte();
    switch (byte) {
        case 'S':
            state->action = AC_ROUNDS;
            break;
        case 'X':
            state->action = AC_FROZEN;
            break;
        case 'P':
            if (state->action == AC_PAUSE) {
                state->action = AC_ROUNDS;
                break;
            }
            if (state->action != AC_ROUNDS) {
                return;
            }
            state->action = AC_PAUSE;
            break;
        case 'C':
            state->action = AC_RETURN_HOME;
            break;
        case 'R':
            state->action = AC_RESET;
            break;
        case '?':
            state_print_help(state);
            return;
        default:
            state_print_fail(byte);
            return;
    }
    state_on_action_change(state);
}

void state_update_position(track_state *trackState) {
    if (timers_check_state(trackState, COUNTER_4_HZ)) {
        trackState->last_pos = trackState->pos;
        // All sensors on, could be home field
        if (trackState->sensor_last == SENSOR_ALL) {
            trackState->homeCache++;
            if (trackState->homeCache > 2) {
                if (trackState->pos != POS_START_FIELD) {
                    USART_print("Start field found \n");
                }
                trackState->pos = POS_START_FIELD;
                trackState->homeCache = 2;
            } else {
                char s[sizeof("Start field tick 1\n")];
                sprintf(s, "Start field tick %d\n", trackState->homeCache);
                USART_print(s);
            }
            return;
        }
        trackState->homeCache = 0;
        if (trackState->action == AC_ROUNDS) {
            if ((trackState->pos) != POS_TRACK) {
                USART_print("Start field lost \n");
            }
            trackState->pos = POS_TRACK;
        } else {
            trackState->pos = POS_UNKNOWN;
        }
    }
}

void state_run_loop(track_state *trackState) {
    while (1) {
        state_read_input(trackState);
        state_show(trackState);
        state_update_position(trackState);
        timers_update(trackState->counters);
        action_type oldAction = trackState->action;
        switch (oldAction) {
            case AC_ROUNDS: {
                drive_run(trackState);
                break;
            }
            case AC_RESET: {
                util_reset();
                break;
            }
            default:
                //Do nothing
                break;
        }
        /**
         * If the state changes an action was applied, usually when the 3 rounds were driven and the robot is switched to
         * util_reset
         */
        if (oldAction != trackState->action) {
            if (oldAction == AC_ROUNDS) {
                motor_drive_stop();
            }
            state_on_action_change(trackState);
        }
    }
}

