"""Collection of common functions used across the system."""

import logging
import logging.config
import os
import json
import webbrowser
import wx
import grpc

from .gui import CCFrame

from . import helloWorld_pb2
from . import helloWorld_pb2_grpc

def genText(text, panel):
    """Standard formatting for window text."""
    text = wx.StaticText(panel, label=text)
    font = text.GetFont()
    # font.PointSize += 10
    font = font.Bold()
    text.SetFont(font)
    return text

class ConsoleHandler(logging.StreamHandler):
    """Captures and draws log messages to wx window."""

    def __init__(self, textctrl):
        """"""
        logging.StreamHandler.__init__(self)
        self.textctrl = textctrl

    def emit(self, record):
        """Constructor"""
        msg = self.format(record)
        self.textctrl.WriteText(msg + "\n")
        self.flush()


class CCFrameImpl(CCFrame):
    """This inherits from the wxGlade generated code, and
    we'll do the implementation details here."""

    def __init__(self, *args, **kw):
        super(CCFrameImpl, self).__init__(*args, **kw)
        self.loadConfig()
        self.address = self.config['serverAddress'] + ':' + str(self.config['serverPort'])
        if self.config['logfilename']:
            self.setupLogging(self.config['logfilename'])
        else:
            self.setupLogging()

    def loadConfig(self, filename: str = "configClient.json"):
        """Loads JSON config file from disk."""
        if not os.path.exists(filename):
            print("No config file found, setting default config")
            self.config = {"serverAddress": "localhost", "serverPort": 50051}
        else:
            with open(filename) as configFile:
                self.config = json.load(configFile)

    def setupLogging(self, logFilename: str = "controlClient.log"):
        """Setup logging to file and window output."""
        logConfig = {
            'version': 1,
            'handlers': {
                'fileHandler': {
                    'class': 'logging.FileHandler',
                    'formatter': 'myFormatter',
                    'filename': logFilename
                    },
                'terminalHandler': {
                    'class': 'logging.StreamHandler',
                    'formatter': 'myFormatter',
                    'stream' : 'ext://sys.stdout'
                }
            },
            'loggers':{
                'controlClient': {
                    'handlers': ['fileHandler', 'terminalHandler'],
                    'level': 'INFO',
                }
            },
            'formatters': {
                'myFormatter': {
                    'format': '%(asctime)-15s %(name)s|%(levelname)s| %(message)s',
                    'datefmt': '%Y-%m-%dT%H:%M:%S%z'
                }
            }
        }
        logging.config.dictConfig(logConfig)
        self.log = logging.getLogger('controlClient')
        self.log.setLevel(logging.DEBUG)
        self.log.addHandler(ConsoleHandler(self.logText))


    def onPress(self, event):
        # pylint: disable=unused-argument
        """Test button."""
        # This sends the RPC call:
        self.log.info("Button pressed!")
        with grpc.insecure_channel('localhost:50051') as channel:
            stub = helloWorld_pb2_grpc.helloRPCStub(channel)
            request = helloWorld_pb2.helloRequest(name='Dani')
            response = stub.sendRequest(request)
            self.log.info(response.reply)

    def onDoThing(self, event):  # pylint: disable=unused-argument
        """Menu: File > DoThing. Say hello to the user."""
        wx.MessageBox("Hello again from wxPython")

    def onExit(self, event):  # pylint: disable=unused-argument
        """Menu: File > Quit."""
        self.Close(True)

    def onHelp(self, event):  # pylint: disable=unused-argument
        """Menu: Help > Help. Open online help in browser."""
        webbrowser.open('https://bensmaialab.github.io/')

    def onAbout(self, event): # pylint: disable=unused-argument
        """Display an About Dialog"""
        wx.MessageBox(
            "BensmaiaLab Control Client GUI.",
            "About Control Client",
            wx.OK|wx.ICON_INFORMATION)


class ControlClientApp(wx.App):
    """Instance the Control Client App."""
    def OnInit(self):
        self.ccframe = CCFrameImpl(None, wx.ID_ANY, "")
        self.SetTopWindow(self.ccframe)
        self.ccframe.Show()
        return True
