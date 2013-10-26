#!/usr/bin/python

import smbus
import time

bus = smbus.SMBus(1)

addr = 0x21

def write(text):
    length = len(text)
    for x in range(0, length):
        single(text[x])

def single(char):
    bus.write_byte_data(addr, 0x00, ord(char))

def row(row):
    bus.write_byte_data(addr, 0x03, row)
    time.sleep(0.01)

def clear():
    bus.write_byte(addr, 0x01)
    time.sleep(0.01)

clear()
write('Countdown!')
row(1)

x = 0
while(1):
    row(1)
    write(str(x)) 
    x+=1
    time.sleep(1)
