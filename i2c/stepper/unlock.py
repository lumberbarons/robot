#!/usr/bin/python

import smbus

bus = smbus.SMBus(1)
address = 0x21
address2 = 0x20

bus.write_byte_data(address, 0x02, 0x00)
bus.write_byte_data(address2, 0x02, 0x00)
