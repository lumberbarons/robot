#include <htc.h>

#include "config.h"
#include "serial.h"
#include "delay.h"
#include "motor.h"

#define MTR_ONE_FWD 0xFF
#define MTR_ONE_BWD 0xFE
#define MTR_ONE_OFF 0xFD
#define MTR_TWO_FWD 0xFC
#define MTR_TWO_BWD 0xFB
#define MTR_TWO_OFF 0xFA

const unsigned char address = 0x20;

void init_i2c() {
    TRISB0 = 1;
    TRISB1 = 1;

    SMP = 0;
    SSPCON1 = 0b00101110; // I2C Enabled, I2C Slave 7-bit Address
    SSPCON2 = 0b00000001; // Clock Stretching Enabled
    SSPADD = address << 1;

    SSPIE = 1;
    PEIE = 1;
    GIE = 1;
}

void init() {
    init_i2c();
    init_motor();

    TRISB4 = 0;
    LATB4 = 1;
    delay_s(1);
    LATB4 = 0;
}

#define MSTR_WRITE_ADD  0b00001001 // Master Write, Last Byte was Address
#define MSTR_WRITE_DATA 0b00101001 // Master Write, Last Byte was Data
#define MSTR_READ_ADD   0b00001101 // Master Read, Last Byte as Address
#define MSTR_READ_DATA  0b00101100 // Master Read, Last Byte was Data

volatile unsigned char sspstat;

//#define I2C_TX_BUFF_SIZE 4
#define I2C_RX_BUFF_SIZE 3

volatile unsigned char i2c_rx_buff[I2C_RX_BUFF_SIZE] = {0x00, 0x00, 0x00};
volatile unsigned char i2c_rx_index;

//volatile unsigned char i2c_tx_buff[I2C_TX_BUFF_SIZE] = {0x00, 0x00, 0x00, 0x00};
// unsigned char i2c_tx_index = 0;

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
            SSPBUF = 0; //i2c_tx_buff[i2c_tx_index];
            CKP = 1;
            //if (++i2c_tx_index == sizeof (i2c_tx_buff)) {
            //    i2c_tx_index = 0;
            //}
        } else if (sspstat == MSTR_READ_DATA && !CKP) {
            SSPBUF = 0; //i2c_tx_buff[i2c_tx_index];
            CKP = 1;
            //if (++i2c_tx_index == sizeof (i2c_tx_buff)) {
            //    i2c_tx_index = 0;
            //}
        } else {
            CKP = 1;
            if (i2c_rx_index > 0) {
                i2c_rx_index = 0;
                ready = 1;
            }
        }
    }
}

unsigned char command;

#define COMMAND_STEP_FWD    0x00
#define COMMAND_STEP_BWD    0x01

void main() {
    init();

    while (1) {
        if (ready) {
            command = i2c_rx_buff[0];
            switch(command) {
                case COMMAND_STEP_FWD:
                    forward();
                    break;
                case COMMAND_STEP_BWD:
                    backward();
                    break;
            }

            ready = 0;
        }
    }
}