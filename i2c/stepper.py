#!/usr/bin/python

import smbus
import time

bus = smbus.SMBus(1)
rotation = 800
address = 0x20

for i in range(0, rotation):
    bus.write_byte_data(address, 0x00, 0x00)
    time.sleep(0.001)

time.sleep(0.1)

for i in range(0, rotation):
    bus.write_byte_data(address, 0x01, 0x00)
    time.sleep(0.001)