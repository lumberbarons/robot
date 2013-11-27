import pykka
import smbus
import time

class LcdWriter(pykka.ThreadingActor):

    bus = smbus.SMBus(1)
    addr = 0x21

    lcdBacklightState = False

    def write(self, text):
        length = len(text)
        for x in range(0, length):
            self._single(text[x])

    def _single(self, char):
        self.bus.write_byte_data(self.addr, 0x00, ord(char))

    def row(self, row):
        self.bus.write_byte_data(self.addr, 0x03, row)
        time.sleep(0.01)

    def clear(self):
        self.bus.write_byte(self.addr, 0x01)
        time.sleep(0.01)
        
    def backlightOn(self):
        self.bus.write_byte(self.addr, 0x04)
        time.sleep(0.01)
        
    def backlightOff(self):
        self.bus.write_byte(self.addr, 0x05)
        time.sleep(0.01)