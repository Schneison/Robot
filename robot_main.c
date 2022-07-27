#include "robot_main.h"

void setup(void) {
    motor_clear();
    ADC_clear();

    USART_init(UBRR_SETTING);
    ADC_init();
    motor_init();
    led_init();
    timers_init();
}

int main(void) {
    //Clear old state and new setup
    setup();

    //Create track state
    track_state *trackState = malloc(sizeof(track_state));
    trackState->drive = DS_CHECK_START;
    trackState->action = AC_WAIT;
    trackState->pos = POS_UNKNOWN;
    trackState->last_pos = POS_UNKNOWN;
    trackState->homeCache = 0;
    // Create counters, has to be done before first use
    timers_create(trackState->counters);
    state_run_loop(trackState);
    //Should never be reached, because #state_run_loop contains an endless loop
    free(trackState);
}
