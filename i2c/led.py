#!/usr/bin/python

import smbus
import time
import font8x8

bus = smbus.SMBus(0)

while True:
    var = raw_input('Letter:')
    asc = ord(var[0])
    bus.write_i2c_block_data(0x27, 0x00, font8x8.letter(asc))
