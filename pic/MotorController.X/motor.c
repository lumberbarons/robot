#include <htc.h>
#include "delay.h"
#include "motor.h"

#define SW_1_E   LATA0
#define SW_2_E   LATA1
#define SW_3_E   LATA2
#define SW_4_E   LATA3

#define HBRIDGE_12_E  LATA4
#define HBRIDGE_34_E  LATA5

void init_motor() {
    TRISA = 0x00;

    SW_1_E = 0;
    SW_2_E = 0;
    SW_3_E = 0;
    SW_4_E = 0;

    HBRIDGE_12_E = 1;
    HBRIDGE_34_E = 1;

    // Timer 2 for PWM
    T2CKPS0 = 0;
    T2CKPS1 = 0;
    PR2 = 0xFF;
    TMR2ON = 1;

    // CCP1 PWM
    TRISC2 = 0;
    CCP1M3 = 1;
    CCP1M2 = 1;
    DC1B1 = 0;
    DC1B0 = 0;
    CCPR1L = 0x00;
    
    // CCP2 PWM
    TRISC1 = 0;
    CCP2M3 = 1;
    CCP2M2 = 1;
    DC2B1 = 0;
    DC2B0 = 0;
    CCPR2L = 0x00;
}

void motor_one_speed(unsigned char speed) {
    CCPR1L = speed;
}

void motor_two_speed(unsigned char speed) {
    CCPR2L = speed;
}

bit m1fwd = 0;
bit m1bwd = 0;

void motor_one_stop() {
    SW_1_E = 0;
    SW_2_E = 0;

    motor_one_speed(0);

    m1fwd = 0;
    m1bwd = 0;
}

void motor_one_forward(unsigned char speed) {
    if(m1bwd == 1) {
        motor_one_speed(0);
        __delay_ms(10);
    }

    SW_2_E = 0;
    motor_one_speed(speed);
    SW_1_E = 1;

    m1fwd = 1;
    m1bwd = 0;
}

void motor_one_backward(unsigned char speed) {
    if(m1fwd == 1) {
        motor_one_speed(0);
        __delay_ms(10);
    }

    SW_1_E = 0;
    motor_one_speed(speed);
    SW_2_E = 1;

    m1fwd = 0;
    m1bwd = 1;
}

void motor_two_stop() {
    SW_3_E = 0;
    SW_4_E = 0;
}

void motor_two_forward(unsigned char speed) {
    SW_3_E = 1;
    SW_4_E = 0;
    motor_two_speed(speed);
}

void motor_two_backward(unsigned char speed) {
    SW_3_E = 0;
    SW_4_E = 1;
    motor_two_speed(speed);
}