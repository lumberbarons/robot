import sys
import signal

from lcd_rpc import LcdRpc
from stepper_rpc import StepperRpc
from lcd import LcdWriter
from stepper import StepperWriter

from twisted.python import log
from twisted.internet import reactor, defer
from twisted.web.server import Site
from twisted.web.static import File
 
from autobahn.websocket import listenWS
from autobahn.wamp import exportRpc, \
                          WampServerFactory, \
                          WampServerProtocol

class RobotServerProtocol(WampServerProtocol):

    def onSessionOpen(self):
        self.registerForRpc(self.factory.lcd, "/lcd#")
        self.registerForPubSub("/lcd#", True)
        self.registerForRpc(self.factory.stepper, "/stepper#")
        self.registerForPubSub("/stepper#", True)

class RobotServerFactory(WampServerFactory):

    protocol = RobotServerProtocol
    
    def __init__(self, wsuri):
        WampServerFactory.__init__(self, wsuri)
        self.stepperWriter = StepperWriter.start().proxy()
        self.lcdWriter = LcdWriter.start().proxy()
        self.lcd = LcdRpc(self, self.lcdWriter);
        self.stepper = StepperRpc(self, self.stepperWriter, self.lcdWriter);
     
if __name__ == '__main__':
    factory = RobotServerFactory("ws://localhost:9000")
    factory.setProtocolOptions(allowHixie76 = True)
    listenWS(factory)
    reactor.run()