#include <htc.h>

#include "config.h"
#include "delay.h"
#include "voltmeter.h"

const unsigned char address = 0x10;

void init_i2c() {
    TRISB0 = 1;
    TRISB1 = 1;

    SSPSTATbits.SMP = 0;
    SSPCON1 = 0b00101110; // I2C Enabled, I2C Slave 7-bit Address
    SSPCON2 = 0b00000001; // Clock Stretching Enabled
    SSPADD = address << 1;

    PIE1bits.SSPIE = 1;
}

void init_timer() {
    T0CON = 0X00;
    INTCONbits.T0IE = 1;
    INTCON2bits.TMR0IP = 0x00; // low priority      
    T0CONbits.TMR0ON = 1;
    T0CONbits.T0PS = 0b111;
    TMR0 = 0xFF;
}

void flash_led() {
    TRISB4 = 0;
    LATB4 = 1;
    delay_10ms(10);
    LATB4 = 0;
}

void init() {
    init_i2c();
    init_timer();
    
    // enable interrupts
    RCONbits.IPEN = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    init_voltmeter();
    flash_led();
}

#define MSTR_WRITE_ADDR  0b00001001 // Master Write, Last Byte was Address
#define MSTR_WRITE_DATA  0b00101001 // Master Write, Last Byte was Data
#define MSTR_READ_ADDR   0b00001101 // Master Read, Last Byte as Address
#define MSTR_READ_DATA   0b00101100 // Master Read, Last Byte was Data

volatile unsigned char sspstat;

#define I2C_TX_BUFF_SIZE 8
#define I2C_RX_BUFF_SIZE 3

volatile unsigned char i2c_rx_buff[I2C_RX_BUFF_SIZE] = {0x00, 0x00, 0x00};
volatile unsigned char i2c_rx_index;

volatile unsigned char i2c_tx_buff[I2C_TX_BUFF_SIZE] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
volatile unsigned char i2c_tx_index = 0;

volatile bit command_ready = 0;

unsigned char get() {
    if(i2c_tx_index >= sizeof (i2c_tx_buff)) {
        return 0xFF;
    }
    return i2c_tx_buff[i2c_tx_index];
}

void interrupt high_isr() {
    if (SSPIF && SSPIE) {            
        SSPIF = 0;
        sspstat = SSPSTAT & 0b00101101; // D/A, S, R/W, BF
        
        if (sspstat == MSTR_WRITE_ADDR) {
            i2c_rx_index = 0;
            SSPBUF;
            CKP = 1;
        } else if (sspstat == MSTR_WRITE_DATA) {
            i2c_rx_buff[i2c_rx_index] = SSPBUF;
            CKP = 1;
            if (++i2c_rx_index == sizeof (i2c_rx_buff)) {
                i2c_rx_index = 0;
            }
        } else if (sspstat == MSTR_READ_ADDR) {
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
                // it's a command
                command_ready = 1; 
            } else {
                // set the index for a get
                i2c_tx_index = i2c_rx_buff[0];
            }
        }
    }
}

void read_voltages() {
    unsigned char channelByte = 0;
    for(unsigned char channel = 0; channel < 4; channel++) {
        unsigned int voltage = read_voltage(channel);
        i2c_tx_buff[channelByte++] = (voltage >> 8) & 0xFF;
        i2c_tx_buff[channelByte++] = voltage & 0xFF;
    }
}

void interrupt low_priority low_isr() {
    if(INTCONbits.T0IF && INTCONbits.T0IE) {                               
        INTCONbits.T0IF = 0;
        read_voltages();
    }
}

void main() {
    init();
    while (1);
}
