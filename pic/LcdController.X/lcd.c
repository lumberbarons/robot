#include <htc.h>

#include "lcd.h"
#include "delay.h"

#define RS  LATB7
#define RW  LATB6
#define E   LATB5

#define D4  LATA0
#define D5  LATA1
#define D6  LATA2
#define D7  LATA3

#define BACKLIGHT LATC0

const char* SPLASH_MSG = "LCDv0.0.1 (2x16)";

void pulse();
void send_command(unsigned char data);
void send_character(unsigned char data);
unsigned char test(unsigned char data, unsigned char shift);
void nibble(unsigned char data);
void splash(const char* message);

void init_lcd() {
    TRISB7 = 0;
    TRISB6 = 0;
    TRISB5 = 0;

    TRISA0 = 0;
    TRISA1 = 0;
    TRISA2 = 0;
    TRISA3 = 0;

    RS = 0;
    RW = 0;
    E = 0;

    delay_s(1);

    nibble(0x03);
    __delay_ms(5);
    nibble(0x03);
    __delay_ms(5);
    nibble(0x03);
    delay_10us(15);
    nibble(0x02);

    send_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);

    TRISC0 = 0;
    backlight_on();

    clear();
    home();

    splash(SPLASH_MSG);

    clear();
    home();

    backlight_off();
}

void splash(const char* message) {
    delay_s(1);
    while(*message != 0) {
        send_character(*message++);
    }
    delay_s(2);
}

void backlight_on() {
    BACKLIGHT = 1;
}

void backlight_off() {
    BACKLIGHT = 0;
}

void pulse() {
    E = 0;
    __delay_us(5);
    E = 1;
    __delay_us(5);
    E = 0;
    delay_10us(10);
}

void clear() {
    send_command(LCD_CLEARDISPLAY);
}

void home() {
    send_command(LCD_RETURNHOME);
}

unsigned char offset = 0;

void cursor(unsigned char col, unsigned char row) {
    offset = 0;
    if(row == 1) {
        offset = 0x40;
    }
    send_command(LCD_SETDDRAMADDR | offset + col);
}

void send_command(unsigned char data) {
    nibble(data >> 4);
    nibble(data);
}

void send_character(unsigned char data) {
    RS = 1;
    nibble(data >> 4);
    nibble(data);
    RS = 0;
}

void nibble(unsigned char data) {
    D4 = test(data, 0);
    D5 = test(data, 1);
    D6 = test(data, 2);
    D7 = test(data, 3);
    pulse();
}

unsigned char test(unsigned char data, unsigned char shift) {
    return data >> shift & 0x01;
}
