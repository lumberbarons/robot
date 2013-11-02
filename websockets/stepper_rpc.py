from autobahn.wamp import exportRpc
from stepper import StepperWriter

class StepperRpc:

    stepperWriter = StepperWriter()
    
    def __init__(self, parent):
        self.parent = parent
        
    @exportRpc("forward")
    def forward(self):
        self.stepperWriter.forward()
        return "success"
    
    @exportRpc("backward")
    def backward(self):
        self.stepperWriter.backward()
        return "success"