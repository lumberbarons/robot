#!/usr/bin/python

import smbus
import time
import imaplib
import sys

address = str(sys.argv[1])
password = str(sys.argv[2])
bus = smbus.SMBus(1)
addr = 0x21

def unread():
    mail = imaplib.IMAP4_SSL('imap.gmail.com')
    mail.login(address, password)
    mail.select("inbox")
    result, data = mail.uid('search', None, 'UNSEEN')
    mail.close()
    uid_list = data[0].split()
    return str(len(uid_list))

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
write('Unread emails:')
row(1)

while(1):
    row(1)
    write(unread()) 
    time.sleep(60)
