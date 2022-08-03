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
                char s[sizeof("Round and round I go, currently round #1")];
                sprintf(s, "Round and round I go, currently round #%d", round);
                usart_print_pretty(s);
            }
            //TODO: Remove before deployment
            led_sensor(state->sensor_last);
            break;
        }
        case AC_FROZEN:
            timers_print(state->counters, COUNTER_1_HZ,
                         "In safe state! Won't react to any instructions! Rescue me!\n");
            if (timers_check_state(state, COUNTER_32_HZ)) {
                led_chase(&(state->last_led));
            }
            break;
        case AC_MANUAL:
            led_sensor(state->sensor_last);
            break;
        case AC_RETURN_HOME:
            timers_print(state->counters, COUNTER_1_HZ,
                         "Returning home, will reset me there\n");
            break;
        case AC_PAUSE:
            timers_print(state->counters, COUNTER_1_HZ,
                         "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            if (timers_check_state(state, COUNTER_2_HZ)) {
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
        usart_println("Currently on track, no help is given if the roboter already "
                      "started driving!");
        return;
    }
    if (state->pos == POS_START_FIELD) {
        usart_println("On the starting field the following actions are valid:");
        usart_println(" - S: 3 Rounds");
        usart_println(" - P: Pause");
        usart_println(" - C: Home");
    } else {
        usart_println("Not on the starting field the following actions are valid:");
    }
    usart_println(" - X: Safe State / Freeze");
    usart_println(" - R: Reset");
    usart_println(" - ?: Help");
    usart_println(" - M: Manual drive");
    usart_println(" -- W: Drive forward");
    usart_println(" -- B: Drive backwards");
    usart_println(" -- A: Drive left");
    usart_print_pretty(" -- D: Drive right");
}

void state_on_action_change(track_state *state, action_type oldAction) {
    if (oldAction == AC_ROUNDS) {
        motor_drive_stop();
    }
    switch (state->action) {
        case AC_RESET:
            usart_print_pretty("Will reset in 5 seconds...");
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
}

void state_read_input(track_state *state) {
    if (!usart_can_receive()) {
        return;
    }
    if (state->action == AC_FROZEN || state->action == AC_RESET) {
        return;
    }
    action_type oldAction = state->action;
    unsigned char byte = usart_receive_byte();
    switch (byte) {
        case 'S':
            if((state->pos) != POS_START_FIELD){
                usart_print_pretty("Can't start when not on the starting field!");
                return;
            }
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
                usart_print_pretty("Not driving on track, can't be paused!");
                return;
            }
            state->action = AC_PAUSE;
            break;
        case 'C':
            if(state->action != AC_ROUNDS){
                usart_print_pretty("Not driving on track, can't be called home!");
                return;
            }
            state->action = AC_RETURN_HOME;
            break;
        case 'M':
            if(state->action){
                state->action = AC_WAIT;
                break;
            }
            state->action = AC_MANUAL;
            break;
        case 'Y':
            state->ui_connection = UI_CONNECTED;
            return;
        case 'Q':
            state->ui_connection = UI_DISCONNECTED;
            return;
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
    if(oldAction == state->action){
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
            if (trackState->pos != POS_START_FIELD && DEBUG) {
                usart_println("Start field found");
            }
            trackState->pos = POS_START_FIELD;
            trackState->homeCache = 3;
        } else if(DEBUG) {
            char s[sizeof("Start field tick 1")];
            sprintf(s, "Start field tick %d", trackState->homeCache);
            usart_println(s);
        }
        return;
    }
    trackState->homeCache = 0;
    if (trackState->action == AC_ROUNDS) {
        if ((trackState->pos) != POS_TRACK && DEBUG) {
            usart_println("Start field lost");
        }
        trackState->pos = POS_TRACK;
    } else {
        trackState->pos = POS_UNKNOWN;
    }
}

void state_send_update(const track_state *trackState) {
    if (trackState->ui_connection == UI_CONNECTED && timers_check_state(trackState,
                                                                        COUNTER_12_HZ)) {
        char* s = "[(7,7,7,7,1000,7)]\n";
        sprintf(s, "[(%d,%d,%d,%d,%d,%d)]\n",
                // Last sensor state
                trackState->sensor_last,
                // Direction of driving
                trackState->dir_last,
                // Current action
                trackState->action,
                // On start field
                trackState->pos == POS_START_FIELD,
                // Is manual
                trackState->action == AC_MANUAL,
                // Battery voltage in percent times 100
                sensor_get_battery());
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
        action_type action = trackState->action;
        switch (action) {
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
         * If the state changes an action was applied, usually when the 3 rounds were driven and the
         * robot is switched to util_reset
         */
        if (action != trackState->action) {
            state_on_action_change(trackState, action);
        }
    }
}

