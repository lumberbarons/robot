from autobahn.wamp import exportRpc

class LcdRpc:

    lcdBacklightState = False
    
    def __init__(self, parent, lcdWriter):
        self.parent = parent
        self.lcdWriter = lcdWriter
        self.lcdWriter.backlightOff()
      
    @exportRpc("write")
    def write(self, text):
        self.lcdWriter.write(text)
        return "success"
   
    @exportRpc("clear")
    def clear(self):
        self.lcdWriter.clear()
        return "success"
      
    @exportRpc("row")
    def row(self, row):
        self.lcdWriter.row(row)
        return "success"

    @exportRpc("backlightState")
    def backlightState(self):
        return LcdRpc.lcdBacklightState

    @exportRpc("backlight")
    def backlight(self, on):
        LcdRpc.lcdBacklightState = on
        if(on):
            self.lcdWriter.backlightOn()
        else:
            self.lcdWriter.backlightOff()
        self.parent.dispatch("/lcd#backlight", on);
        return "success"