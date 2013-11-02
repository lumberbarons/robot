#include <xc.h>

#include "config.h"
#include "delay.h"
#include "lcd.h"

#include <plib/usart.h>

#

unsigned char NEWLINE[] = "\r\n";

const unsigned char address = 0x21;

void init_i2c() {
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;

    SMP = 0;
    SSPCON1 = 0b00101110; // I2C Enabled, I2C Slave 7-bit Address
    SSPCON2 = 0b00000001; // Clock Stretching Enabled
    SSPADD = address << 1;

    PIE1bits.SSPIE = 1;
    PEIE = 1;
    GIE = 1;
}

unsigned char UART1Config = 0;
unsigned char baud = 312; // 9600 Baud at 48MHz

void init_serial() {
    TRISCbits.RC6 = 0;
    TRISCbits.RC7 = 1;

    UART1Config = USART_TX_INT_OFF & USART_TX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_BRGH_HIGH;
    OpenUSART(UART1Config, baud);

    putsUSART(SPLASH_MSG);
}

void init() {
    init_lcd();
    init_i2c();

    init_serial();
}

#define MSTR_WRITE_ADD  0b00001001 // Master Write, Last Byte was Address
#define MSTR_WRITE_DATA 0b00101001 // Master Write, Last Byte was Data
#define MSTR_READ_ADD   0b00001101 // Master Read, Last Byte as Address
#define MSTR_READ_DATA  0b00101100 // Master Read, Last Byte was Data

volatile unsigned char sspstat;

#define I2C_TX_BUFF_SIZE 33 // 2 x 16 + backlight state
#define I2C_RX_BUFF_SIZE 3

#define I2C_TX_BUFF_ROW_ONE     0
#define I2C_TX_BUFF_ROW_TWO     16
#define I2C_TX_BUFF_BACKLIGHT   32

volatile unsigned char i2c_tx_buff[I2C_TX_BUFF_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
volatile unsigned char i2c_tx_index = 0;

volatile unsigned char i2c_rx_buff[I2C_RX_BUFF_SIZE] = {0x00, 0x00, 0x00};
volatile unsigned char i2c_rx_index;

volatile bit start = 0;
volatile bit ready = 0;

void interrupt isr() {
    if (SSPIF && SSPIE) {
        SSPIF = 0;
        sspstat = SSPSTAT & 0b00101101; // D/A, S, R/W, BF

        if (sspstat == MSTR_WRITE_ADD) {
            i2c_rx_index = 0;
            SSPBUF;
            CKP = 1;
        } else if (sspstat == MSTR_WRITE_DATA) {
            i2c_rx_buff[i2c_rx_index] = SSPBUF;
            CKP = 1;
            if (++i2c_rx_index == sizeof (i2c_rx_buff)) {
                i2c_rx_index = 0;
            }
        } else if (sspstat == MSTR_READ_ADD) {
            SSPBUF;
            SSPBUF = i2c_tx_buff[i2c_tx_index];
            CKP = 1;
            if (++i2c_tx_index == sizeof (i2c_tx_buff)) {
                i2c_tx_index = 0;
            }
        } else if (sspstat == MSTR_READ_DATA && !CKP) {
            SSPBUF = i2c_tx_buff[i2c_tx_index];
            CKP = 1;
            if (++i2c_tx_index == sizeof (i2c_tx_buff)) {
                i2c_tx_index = 0;
            }
        } else {
            CKP = 1;
            if (i2c_rx_index > 0) {
                i2c_rx_index = 0;
                ready = 1;
            }
        }
    }
}

#define COMMAND_TEXT        0x00
#define COMMAND_CLEAR       0x01
#define COMMAND_HOME        0x02
#define COMMAND_CURSOR      0x03
#define COMMAND_BL_ON       0x04
#define COMMAND_BL_OFF      0x05
#define COMMAND_RESET_READ  0x06

unsigned char command;

void main() {
    init();

    while (1) {
        if (ready) {
            command = i2c_rx_buff[0];
            switch(command) {
                case COMMAND_TEXT:
                    send_character(i2c_rx_buff[1]);
                    break;
                case COMMAND_CLEAR:
                    clear();
                    break;
                case COMMAND_HOME:
                    home();
                    break;
                case COMMAND_CURSOR:
                    cursor(0, i2c_rx_buff[1]);
                    break;
                case COMMAND_BL_ON:
                    backlight_on();
                    break;
                case COMMAND_BL_OFF:
                    backlight_off();
                    break;
                case COMMAND_RESET_READ:
                    i2c_tx_index = 0;
                    break;
            }

            ready = 0;
        }
    }
}