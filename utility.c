#include "utility.h"

_Noreturn void util_reset(void) {
    //Enables the watch dog timer
    wdt_enable(WATCH_DOG_TIME);
    while (1) {
        //Endless loop to trigger the watch dog timer
    }
    // Never reached
    wdt_disable();
}