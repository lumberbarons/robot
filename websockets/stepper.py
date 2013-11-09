import smbus
import time

class StepperWriter:

    addr = 0x20
    bus = smbus.SMBus(1)
    
    rotation = 200

    def forward(self):
        for i in range(0, self.rotation):
            self.bus.write_byte_data(self.addr, 0x00, 0x00)
            time.sleep(0.002)
        time.sleep(0.005)
        self.bus.write_byte_data(self.addr, 0x02, 0x00)
            
    def backward(self):
        for i in range(0, self.rotation):
            self.bus.write_byte_data(self.addr, 0x01, 0x00)
            time.sleep(0.002)
        time.sleep(0.005)
        self.bus.write_byte_data(self.addr, 0x02, 0x00)