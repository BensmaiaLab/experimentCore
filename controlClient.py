# /usr/bin/python3
# pylint: disable=invalid-name
""" GUI based controller client for bensmaialab."""

# controlClient.py
import argparse
import webbrowser
import logging
import logging.config
import json
import os

import wx

import grpc
import pyClient.helloWorld_pb2
import pyClient.helloWorld_pb2_grpc


# This sends the RPC call:
with grpc.insecure_channel('localhost:50051') as channel:
    stub = pyClient.helloWorld_pb2_grpc.helloRPCStub(channel)
    request = pyClient.helloWorld_pb2.helloRequest(name='Dani')
    response = stub.sendRequest(request)
    print(response.reply)
    exit()

def genText(text, panel):
    """Standard formatting for window text."""
    text = wx.StaticText(panel, label=text)
    font = text.GetFont()
    # font.PointSize += 10
    font = font.Bold()
    text.SetFont(font)
    return text


class ConsoleLogHandler(logging.StreamHandler):
    """Captures and draws log messages to window."""

    def __init__(self, textctrl):
        """"""
        logging.StreamHandler.__init__(self)
        self.textctrl = textctrl

    def emit(self, record):
        """Constructor"""
        msg = self.format(record)
        self.textctrl.WriteText(msg + "\n")
        self.flush()


class MainFrame(wx.Frame):
    """
    A Frame that says Hello World
    """

    def __init__(self, *args, **kw):
        # ensure the parent's __init__ is called
        super(MainFrame, self).__init__(*args, **kw)

        # create a panel in the frame
        mainPanel = wx.Panel(self)

        # Get config and setup logging
        self.loadConfig()
        if self.config['logfilename']:
            self.setupLogging(self.config['logfilename'])
        else:
            self.setupLogging()

        # and create a sizer to manage the layout of child widgets
        statusSizer = wx.BoxSizer(wx.HORIZONTAL)
        statusSizer.Add(genText("Server Address", mainPanel), 0)
        serverAddress = wx.StaticText(mainPanel, label=self.config['serverAddress'])
        statusSizer.Add(serverAddress, 0)

        logSizer = wx.BoxSizer(wx.HORIZONTAL)
        # Log display window
        logText = wx.TextCtrl(self, style=wx.TE_MULTILINE|wx.TE_READONLY|wx.HSCROLL)
        logSizer.Add(logText, 1, wx.EXPAND|wx.ALL, 5)
        logSizer.SetMinSize(300, 500)

        # Test button
        btn = wx.Button(self, label="Press Me")
        btn.Bind(wx.EVT_BUTTON, self.onPress)
        logSizer.Add(btn, 0, wx.ALL, 5)

        # Add all controls to panel
        topSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(statusSizer)
        topSizer.Add(logSizer)
        mainPanel.SetSizer(topSizer)
        # topSizer.Fit(self)
        txtHandler = ConsoleLogHandler(logText)
        self.log.addHandler(txtHandler)

        # create a menu bar
        self.makeMenuBar()

        # and a status bar
        self.CreateStatusBar()
        self.SetStatusText("Welcome to wxPython!")
        self.log.info("Main window rendered!")

    def loadConfig(self, filename: str = "configClient.json"):
        """Loads JSON config file from disk."""
        if not os.path.exists(filename):
            self.log.info("No config file found, setting default config")
            self.config = {"serverAddress": "localhost", "serverPort": 42000}
        else:
            with open(filename) as configFile:
                self.config = json.load(configFile)

    def onPress(self, event):
        # pylint: disable=unused-argument
        """Test button."""
        self.log.error("Error Will Robinson!")
        self.log.info("Informational message")

    def setupLogging(self, logFilename: str = "controlClient.log"):
        """Setup logging to file and window output."""
        dictLogConfig = {
            'version': 1,
            'handlers': {
                'fileHandler': {
                    'class': 'logging.FileHandler',
                    'formatter': 'myFormatter',
                    'filename': logFilename
                    },
                'consoleHandler': {
                    'class': 'logging.StreamHandler',
                    'formatter': 'myFormatter'
                }
            },
            'loggers':{
                'wxApp': {
                    'handlers': ['fileHandler', 'consoleHandler'],
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
        logging.config.dictConfig(dictLogConfig)
        self.log = logging.getLogger('controlClient')
        self.log.setLevel(logging.DEBUG)

    def makeMenuBar(self):
        """
        This method builds a set of menus and binds handlers to be called
        when the menu item is selected.
        """

        fileMenu = wx.Menu()
        # The "\t..." syntax defines an accelerator key that also triggers
        # the same event
        helloItem = fileMenu.Append(
            -1,
            "&Hello...\tCtrl-H",
            "Help string shown in status bar for this menu item")
        fileMenu.AppendSeparator()
        exitItem = fileMenu.Append(wx.ID_EXIT)

        # Now a help menu for the about item
        helpMenu = wx.Menu()
        helpItem = helpMenu.Append(-1, "Online &Help...\tCtrl-H", "Access online help files.")
        aboutItem = helpMenu.Append(wx.ID_ABOUT)

        # Make the menu bar and add the two menus to it. The '&' defines
        # that the next letter is the "mnemonic" (shortcut key) for the menu item.
        menuBar = wx.MenuBar()
        menuBar.Append(fileMenu, "&File")
        menuBar.Append(helpMenu, "&Help")

        # Give the menu bar to the frame
        self.SetMenuBar(menuBar)

        # Finally, associate a handler function with the EVT_MENU event for
        # each of the menu items.
        self.Bind(wx.EVT_MENU, self.onHello, helloItem)
        self.Bind(wx.EVT_MENU, self.onHelp, helpItem)
        self.Bind(wx.EVT_MENU, self.onExit, exitItem)
        self.Bind(wx.EVT_MENU, self.onAbout, aboutItem)

    def onExit(self, event):
        # pylint: disable=unused-argument
        """Close the frame, terminating the application."""
        self.Close(True)

    @staticmethod
    def onHello(event):
        # pylint: disable=unused-argument
        """Say hello to the user."""
        wx.MessageBox("Hello again from wxPython")

    @staticmethod
    def onAbout(event):
        # pylint: disable=unused-argument
        """Display an About Dialog"""
        wx.MessageBox("BensmaiaLab Control Client GUI.",
                      "About Control Client",
                      wx.OK|wx.ICON_INFORMATION)

    @staticmethod
    def onHelp(event):
        # pylint: disable=unused-argument
        """Open online help in browser."""
        webbrowser.open('https://bensmaialab.github.io/')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=('''
Control Server for BensmaiaLab.
'''))
    parser.add_argument('--verbose', '-v', action='store_true')
    cliArgs = parser.parse_args()

    app = wx.App()
    frm = MainFrame(None, title='BensmaiaLab Control Client')
    frm.Show()
    app.MainLoop()
