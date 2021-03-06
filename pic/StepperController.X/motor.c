#include <xc.h>
#include "delay.h"
#include "motor.h"

#define HBRIDGE_12_E  LATA4
#define HBRIDGE_34_E  LATA5
#define MOTOR_1       LATC1
#define MOTOR_2       LATC2

#define BLACK         0
#define GREEN         1
#define RED           2
#define BLUE          3

#define bitset(var, bitno) (((var) |= 1UL << (bitno))
#define bitclr(var, bitno) (((var) &= ~(1UL << (bitno)))

void init_motor() {
    TRISA = 0;

    disable();
    
    TRISC1 = 0;
    TRISC2 = 0;

    MOTOR_1 = 1;
    MOTOR_2 = 1;
}

unsigned char forward_step = 0;
const unsigned char forward_steps[4] = { BLACK, RED, GREEN, BLUE };
unsigned char reverse_step = 0;
const unsigned char reverse_steps[4] = { BLUE, GREEN, RED, BLACK };

#define DELAY   1

unsigned char next_step = 0;

bit enabled = 0;

void clear() {
    bitclr(LATA, BLACK);
    bitclr(LATA, GREEN);
    bitclr(LATA, RED);
    bitclr(LATA, BLUE);
}

void disable() {
    clear();
    HBRIDGE_12_E = 0;
    HBRIDGE_34_E = 0;
    enabled = 0;
}

void enable() {
    clear();
    HBRIDGE_12_E = 1;
    HBRIDGE_34_E = 1;
    enabled = 1;
}

unsigned char next(unsigned char step) {
    if(step >= 4) {
        step = step - 4;
    }
    return step;
}

void forward() {
    if(!enabled) enable();
    bitset(LATA, forward_steps[forward_step]);
    bitset(LATA, forward_steps[next(forward_step + 1)]);
    bitclr(LATA, forward_steps[next(forward_step + 2)]);
    bitclr(LATA, forward_steps[next(forward_step + 3)]);;
    forward_step = next(forward_step + 1);
    __delay_ms(DELAY);
}

void backward() {
    if(!enabled) enable();
    bitset(LATA, reverse_steps[reverse_step]);
    bitset(LATA, reverse_steps[next(reverse_step + 1)]);
    bitclr(LATA, reverse_steps[next(reverse_step + 2)]);
    bitclr(LATA, reverse_steps[next(reverse_step + 3)]);;
    reverse_step = next(reverse_step + 1);
    __delay_ms(DELAY);
}