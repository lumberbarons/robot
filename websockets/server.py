import sys

from lcd_rpc import LcdRpc
from stepper_rpc import StepperRpc

from twisted.python import log
from twisted.internet import reactor, defer
from twisted.web.server import Site
from twisted.web.static import File
 
from autobahn.websocket import listenWS
from autobahn.wamp import exportRpc, \
                          WampServerFactory, \
                          WampServerProtocol

class RpcServerProtocol(WampServerProtocol):
 
    def onSessionOpen(self):
        self.lcd = LcdRpc(self);
        self.registerForRpc(self.lcd, "/lcd#")
        self.registerForPubSub("/lcd#", True)
        
        self.stepper = StepperRpc(self);
        self.registerForRpc(self.stepper, "/stepper#")
        self.registerForPubSub("/stepper#", True)
      
if __name__ == '__main__':
    factory = WampServerFactory("ws://localhost:9000")
    factory.protocol = RpcServerProtocol
    factory.setProtocolOptions(allowHixie76 = True)
    listenWS(factory)
    reactor.run()
