#include <htc.h>
#include "voltmeter.h"

unsigned int voltage;

void init_voltmeter() {
    TRISA0 = 1;
    TRISA1 = 1;
    TRISA2 = 1;
    TRISA3 = 1;

    ADCON2 = 0xB5;
    ADON = 1;
}

unsigned int read_voltage(unsigned char channel) {
    ADCON0 = (channel << 2) | 0x01;
    
    GO_DONE = 1;
    while(GO_DONE == 1);

    voltage = ADRESL | (ADRESH << 8);

    return voltage;
}


