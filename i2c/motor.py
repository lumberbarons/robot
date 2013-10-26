#!/usr/bin/python

import smbus
import time

bus = smbus.SMBus(1)

address = 0x20
speed = 0x30
diff = 0x10

direction = 0xFF

while True:
    print "speed", speed
    bus.write_byte_data(address, direction, speed)
    time.sleep(0.05)
    speed += diff
    if speed == 0xF0 or speed == 0x30:
        diff = -diff
    if speed == 0x30:
        if direction == 0xFF:
            direction = 0xFE
        elif direction == 0xFE:
            direction = 0xFF
    

right_forward = bus.read_byte_data(address, 0x00)
right_backward = bus.read_byte_data(address, 0x01)
left_forward = bus.read_byte_data(address, 0x02)
left_backward = bus.read_byte_data(address, 0x03)
