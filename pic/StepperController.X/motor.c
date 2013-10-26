#include <htc.h>
#include "delay.h"
#include "motor.h"

#define HBRIDGE_12_E  LATA4
#define HBRIDGE_34_E  LATA5
#define MOTOR_1       LATC1
#define MOTOR_2       LATC2

void init_motor() {
    TRISA = 0x00;

    HBRIDGE_12_E = 1;
    HBRIDGE_34_E = 1;

    TRISC = 0x00;

    MOTOR_1 = 1;
    MOTOR_2 = 1;
}

unsigned char forward_step = 0;
const unsigned char forward_steps[4] = { 0, 2, 1, 3 };
unsigned char reverse_step = 0;
const unsigned char reverse_steps[4] = { 3, 1, 2, 0 };

#define bitset(var, bitno) (((var) |= 1UL << (bitno))
#define bitclr(var, bitno) (((var) &= ~(1UL << (bitno)))

#define DELAY   6

void forward() {
    bitset(LATA, forward_steps[forward_step]);
    __delay_ms(DELAY);
    bitclr(LATA, forward_steps[forward_step]);
    forward_step++;
    if (forward_step == 4) {
        forward_step = 0;
    }
}

void backward() {
    bitset(LATA, reverse_steps[reverse_step]);
    __delay_ms(DELAY);
    bitclr(LATA, reverse_steps[reverse_step]);
    reverse_step++;
    if (reverse_step == 4) {
        reverse_step = 0;
    }
}