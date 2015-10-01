#include <htc.h>

#include "config.h"
#include "serial.h"
#include "delay.h"
#include "voltmeter.h"

const unsigned char address = 0x22;

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

void flash_led() {
    TRISB4 = 0;
    LATB4 = 1;
    delay_10ms(10);
    LATB4 = 0;
}

void init() {
    init_serial(9600);
    init_i2c();
    init_voltmeter();

    flash_led();
}

#define MSTR_WRITE_ADD  0b00001001 // Master Write, Last Byte was Address
#define MSTR_WRITE_DATA 0b00101001 // Master Write, Last Byte was Data
#define MSTR_READ_ADD   0b00001101 // Master Read, Last Byte as Address
#define MSTR_READ_DATA  0b00101100 // Master Read, Last Byte was Data

volatile unsigned char sspstat;

#define I2C_TX_BUFF_SIZE 8
#define I2C_RX_BUFF_SIZE 3

volatile unsigned char i2c_rx_buff[I2C_RX_BUFF_SIZE] = {0x00, 0x00, 0x00};
volatile unsigned char i2c_rx_index;

volatile unsigned char i2c_tx_buff[I2C_TX_BUFF_SIZE] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
volatile unsigned char i2c_tx_index = 0;

volatile bit start = 0;
volatile bit ready = 0;

unsigned char get() {
    if(i2c_tx_index >= sizeof (i2c_tx_buff)) {
        return 0xFF;
    }
    return i2c_tx_buff[i2c_tx_index];
}

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
            SSPBUF = get();
            CKP = 1;
            i2c_tx_index++;
        } else if (sspstat == MSTR_READ_DATA && !CKP) {
            SSPBUF = get();
            CKP = 1;
            i2c_tx_index++;
        } else {
            CKP = 1;
            if(i2c_rx_index == 2) {
                // it's a command so go go go
                ready = 1;
            } else {
                // set the index for a get
                i2c_tx_index = i2c_rx_buff[0];
            }
        }
    }
}

unsigned int voltage;
unsigned char channel;

void main() {
    init();

    while (1) {
        if (ready) {
            voltage = read_voltage(0);
            i2c_tx_buff[0] = (voltage >> 8) & 0xFF;
            i2c_tx_buff[1] = voltage & 0xFF;

            voltage = read_voltage(1);
            i2c_tx_buff[2] = (voltage >> 8) & 0xFF;
            i2c_tx_buff[3] = voltage & 0xFF;

            voltage = read_voltage(2);
            i2c_tx_buff[4] = (voltage >> 8) & 0xFF;
            i2c_tx_buff[5] = voltage & 0xFF;

            voltage = read_voltage(3);
            i2c_tx_buff[6] = (voltage >> 8) & 0xFF;
            i2c_tx_buff[7] = voltage & 0xFF;

            ready = 0;
        }
    }
}
