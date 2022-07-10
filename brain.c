#include "brain.h"


void reset(void) {
    //TODO: How to do 5 Sek ? Sleep 4 / 1 and then timeout ?
    //Enables the watch dog timer
    wdt_enable(WDTO_1S | WDTO_4S);
    // Endless loop to trigger the watch dog timer
    while (1){};
    // Never reached
    wdt_disable();
}

