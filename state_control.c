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
            // Manual check, so we don't have to create a pointer every tick
            if (timers_check_state(state, COUNTER_1_HZ)) {
                char s[sizeof("Round and round I go, currently round #1\n")];
                sprintf(s, "Round and round I go, currently round #%d\n", round);
                usart_print(s);
            }
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
        case AC_MANUAL:
            led_sensor(state->sensor_last);
            break;
        case AC_RETURN_HOME:
            timers_print(state->counters, COUNTER_1_HZ,
                         "Returning home, will util_reset me there\n");
            break;
        case AC_PAUSE:
            timers_print(state->counters, COUNTER_1_HZ,
                         "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            if (timers_check_state(state, COUNTER_3_HZ)) {
                led_chase(&(state->last_led));
            }
            break;
        case AC_WAIT:
            if ((state->pos) == POS_START_FIELD) {
                timers_print(state->counters, COUNTER_1_HZ,
                             "On the starting field. Waiting for your instructions..."
                             " Send ? for help.\n");
                if (timers_check_state(state, COUNTER_5_HZ)) {
                    led_blink(&(state->last_led));
                }
            } else {
                timers_print(state->counters, COUNTER_1_HZ,
                             "Not on the starting field. Place me there please... "
                             "Send ? for help.\n");
                led_sensor(state->sensor_last);
            }
            break;
        default:
            break;
    }
}

void state_print_help(const track_state *state) {
    //Only print help text if S was not received once
    if (state->has_driven_once) {
        return;
    }
    if (state->pos == POS_START_FIELD) {
        usart_print("-X Safe action_state\n-S 3 Rounds\n-P Pause\n-R Reset\n-C Home\n-? Help\n");
    } else {
        usart_print("Not on start field, please position on start field!\n");
    }
}

void state_on_action_change(track_state *state, action_type oldAction) {
    if (oldAction == AC_ROUNDS) {
        motor_drive_stop();
    }
    switch (state->action) {
        case AC_RESET:
            usart_print("Will util_reset in 5 seconds...\n");
            break;
        case AC_WAIT: //Fallthrough
        case AC_FROZEN: //Fallthrough
        case AC_PAUSE: //Fallthrough
            //Reset chase or blink light
            state->last_led = 0;
            break;
        case AC_ROUNDS:
            state->has_driven_once = 1;
            break;
        default:
            break;
    }
    //TODO: remove
    usart_print("State change....\n");
}

void state_read_input(track_state *state) {
    if (!usart_can_receive()) {
        return;
    }
    if (state->action == AC_FROZEN) {
        return;
    }
    action_type oldAction = state->action;
    unsigned char byte = usart_receive_byte();
    switch (byte) {
        case 'S':
            state->action = AC_ROUNDS;
            break;
        case 'X':
            state->action = AC_FROZEN;
            break;
        case 'P':
            if ((state->action) == AC_PAUSE) {
                state->action = AC_ROUNDS;
                break;
            }
            if ((state->action) != AC_ROUNDS) {
                return;
            }
            state->action = AC_PAUSE;
            break;
        case 'C':
            state->action = AC_RETURN_HOME;
            break;
        case 'M':
            state->action = AC_MANUAL;
            break;
        case 'Y':
            state->ui_connection = UI_CONNECTED;
            break;
        case 'Q':
            state->ui_connection = UI_DISCONNECTED;
            break;
        case 'R':
            state->action = AC_RESET;
            break;
        case '?':
            state_print_help(state);
            return;
        default:
            if (state->action != AC_MANUAL) {
                return;
            }
            switch (byte) {
                case 'W':
                    state->manual_dir = DIR_FORWARD;
                    break;
                case 'A':
                    state->manual_dir = DIR_LEFT;
                    break;
                case 'D':
                    state->manual_dir = DIR_RIGHT;
                    break;
                case 'B':
                    state->manual_dir = DIR_BACK;
                    break;
                default:
                    break;
            }
            return;
    }
    state_on_action_change(state, oldAction);
}

void state_update_position(track_state *trackState) {
    if (!timers_check_state(trackState, COUNTER_12_HZ)) {
        return;
    }
    trackState->last_pos = trackState->pos;
    // All sensors on, could be home field
    if (trackState->sensor_last == SENSOR_ALL) {
        trackState->homeCache++;
        if (trackState->homeCache > 2) {
            if (trackState->pos != POS_START_FIELD) {
                usart_print("Start field found \n");
            }
            trackState->pos = POS_START_FIELD;
            trackState->homeCache = 3;
        } else {
            char s[sizeof("Start field tick 1\n")];
            sprintf(s, "Start field tick %d\n", trackState->homeCache);
            usart_print(s);
        }
        return;
    }
    trackState->homeCache = 0;
    if (trackState->action == AC_ROUNDS) {
        if ((trackState->pos) != POS_TRACK) {
            usart_print("Start field lost \n");
        }
        trackState->pos = POS_TRACK;
    } else {
        trackState->pos = POS_UNKNOWN;
    }
}

void state_send_update(track_state *trackState) {
    if (trackState->ui_connection == UI_CONNECTED && timers_check_state(trackState,
                                                                        COUNTER_3_HZ)) {
        char s[sizeof("[(7,7,7,7,100,7)]\n")];
        sprintf(s, "[(%d,%d,%d,%d,%d,%d)]\n",
                // Last sensor state
                trackState->sensor_last,
                // Direction of driving
                trackState->last_dir,
                // Current action
                trackState->action,
                // On start field
                trackState->pos == POS_START_FIELD,
                // Battery voltage in percent times 100
                sensor_get_battery(),
                // Is manual
                trackState->action == AC_MANUAL);
        usart_print(s);
    }
}

_Noreturn void state_run_loop(track_state *trackState) {
    while (1) {
        state_read_input(trackState);
        trackState->sensor_current = sensor_get_state();
        state_update_position(trackState);
        timers_update(trackState->counters);
        state_show(trackState);
        state_send_update(trackState);
        action_type oldAction = trackState->action;
        switch (oldAction) {
            case AC_MANUAL: {
                drive_manual(trackState);
                break;
            }
            case AC_RETURN_HOME: {
                drive_home(trackState);
                break;
            }
            case AC_ROUNDS: {
                drive_run(trackState);
                break;
            }
            case AC_RESET: {
                util_reset();
            }
            default:
                //Do nothing
                break;
        }
        trackState->sensor_last = trackState->sensor_current;
        /**
         * If the state changes an action was applied, usually when the 3 rounds were driven and the robot is switched to
         * util_reset
         */
        if (oldAction != trackState->action) {
            state_on_action_change(trackState, oldAction);
        }
    }
}

