#include <htc.h>
#include "delay.h"
#include "serial.h"

void send(unsigned char val) {
    while(TXIF == 0) {}
    TXREG = val;
}

void init_serial(unsigned long int baud) {
    TRISC6 = 1;
    TRISC7 = 1;

    int x;
    unsigned long tmp;

    tmp = 16UL * baud;
    x = (int) (_XTAL_FREQ / tmp) - 1;
    if ((x > 255) || (x < 0)) {
        tmp = 64UL * baud;
        x = (int) (_XTAL_FREQ / tmp) - 1;
        if ((x > 255) || (x < 0)) {
            return;
        } else {
            BRGH = 0;
        }
    } else {
        BRGH = 1;
    }
    SPBRG = x;

    SYNC = 0;
    SPEN = 1;
    CREN = 1;
    SREN = 0;
    TXIE = 0;
    RCIE = 0;
    TXEN = 1;

    return;
}