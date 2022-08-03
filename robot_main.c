#include "robot_main.h"

void setup(void) {
    motor_clear();
    sensor_clear();

    usart_init(UBRR_SETTING);
    sensor_init();
    motor_init();
    led_init();
    timers_init();
}

int main(void) {
    //Clear old state and new setup
    setup();

    //Create track state
    track_state trackState;
    trackState.drive = DS_CHECK_START;
    trackState.action = AC_WAIT;
    trackState.pos = POS_UNKNOWN;
    trackState.last_pos = POS_UNKNOWN;
    trackState.homeCache = 0;
    trackState.manual_dir = DIR_NONE;
    trackState.manual_dir_last = 0;
    trackState.ui_connection = UI_DISCONNECTED;
    trackState.has_driven_once = 0;
    trackState.dir_last = DIR_NONE;
    trackState.dir_last_valid = DIR_NONE;
    trackState.dir_last_simple = DIR_LEFT;
    // Create counters, has to be done before first use
    timers_create(trackState.counters);
    state_run_loop(&trackState);
}
