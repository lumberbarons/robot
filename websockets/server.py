import sys

from lcd import LcdWriter

from twisted.python import log
from twisted.internet import reactor, defer
from twisted.web.server import Site
from twisted.web.static import File
 
from autobahn.websocket import listenWS
from autobahn.wamp import exportRpc, \
                          WampServerFactory, \
                          WampServerProtocol

class RpcServerProtocol(WampServerProtocol):
 
   writer = LcdWriter()
   text = ""
   state = True
 
   def onSessionOpen(self):
      self.registerForRpc(self, "/lcd#")
      self.registerForPubSub("/lcd#", True)
 
   @exportRpc("write")
   def write(self, text):
      RpcServerProtocol.text += text
      self.writer.write(text)
      self.dispatch("/lcd#write", RpcServerProtocol.text);
      return "success"
   
   @exportRpc("read")
   def read(self):
      return self.text
   
   @exportRpc("clear")
   def clear(self):
      RpcServerProtocol.text = ""
      self.writer.clear()
      self.dispatch("/lcd#write", RpcServerProtocol.text);
      return "success"
      
   @exportRpc("row")
   def row(self, row):
      self.writer.row(row)
      return "success"

   @exportRpc("backlightState")
   def backlightState(self):
      return RpcServerProtocol.state

   @exportRpc("backlight")
   def backlight(self, on):
      RpcServerProtocol.state = on
      if(on):
         self.writer.backlightOn()
      else:
         self.writer.backlightOff()
      self.dispatch("/lcd#backlight", on);
      return "success"
      
if __name__ == '__main__':
   factory = WampServerFactory("ws://localhost:9000")
   factory.protocol = RpcServerProtocol
   factory.setProtocolOptions(allowHixie76 = True)
   listenWS(factory)
   reactor.run()
