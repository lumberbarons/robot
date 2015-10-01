#!/usr/bin/python

import smbus
import time

bus = smbus.SMBus(1)
rotation = 100
address = 0x20

bus.write_byte_data(address, 0x02, 0x00)

