#!/usr/bin/python

import threading
import time
import smbus

clock = 0x00
counter = 0x01
unlock = 0x02

address1 = 0x20
address2 = 0x21

direction1 = 0x02
direction2 = 0x02

delay = 0.008

bus = smbus.SMBus(1)

global cmd
cmd = 'u'

while True:
    cmd = raw_input('Enter command: ')
    if cmd == 's':
        break
    elif cmd == 'u':
	bus.write_byte_data(address1, unlock, 0x00)
	bus.write_byte_data(address2, unlock, 0x00)
    elif cmd == 'f':
	bus.write_byte_data(address1, clock, 0x00)
        bus.write_byte_data(address2, counter, 0x00)
    elif cmd == 'b':
	bus.write_byte_data(address1, counter, 0x00)
        bus.write_byte_data(address2, clock, 0x00)
    elif cmd == 'r':
	bus.write_byte_data(address1, counter, 0x00)
        bus.write_byte_data(address2, counter, 0x00)
    elif cmd == 'l':
	bus.write_byte_data(address1, clock, 0x00)
        bus.write_byte_data(address2, clock, 0x00)
