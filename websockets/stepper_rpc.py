from autobahn.wamp import exportRpc
from stepper import StepperWriter

class StepperRpc:

    def __init__(self, parent, stepperWriter, lcdWriter):
        self.parent = parent
        self.stepperWriter = stepperWriter
        self.lcdWriter = lcdWriter
        
    @exportRpc("forward")
    def forward(self):
        self.stepperWriter.forward()
        self.lcdWriter.clear()
        self.lcdWriter.write("FORWARD")
        return "success"
    
    @exportRpc("backward")
    def backward(self):
        self.stepperWriter.backward()
        self.lcdWriter.clear()
        self.lcdWriter.write("BACKWARD")
        return "success"