#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# generated by wxGlade 0.9.6 on Fri Aug  7 14:39:24 2020
#

import wx

# begin wxGlade: dependencies
# end wxGlade

# begin wxGlade: extracode
# end wxGlade


class CCFrame(wx.Frame):
    def __init__(self, *args, **kwds):
        # begin wxGlade: CCFrame.__init__
        kwds["style"] = kwds.get("style", 0) | wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, *args, **kwds)
        self.SetSize((495, 348))
        
        # Menu Bar
        self.frame_menubar = wx.MenuBar()
        self.fileMenu = wx.Menu()
        self.frame_menubar.doThingItem = self.fileMenu.Append(wx.ID_ANY, "DoThing", "\"Help string goes here for DoThing\"")
        self.Bind(wx.EVT_MENU, self.onDoThing, id=self.frame_menubar.doThingItem.GetId())
        self.frame_menubar.exitItem = self.fileMenu.Append(wx.ID_EXIT, "Quit", "\"Exit the application.\"")
        self.Bind(wx.EVT_MENU, self.onExit, id=wx.ID_EXIT)
        self.frame_menubar.Append(self.fileMenu, "&File")
        self.helpMenu = wx.Menu()
        item = self.helpMenu.Append(wx.ID_ANY, "&Help (Online)", "Access online help.")
        self.Bind(wx.EVT_MENU, self.onHelp, id=item.GetId())
        self.frame_menubar.aboutItem = self.helpMenu.Append(wx.ID_ABOUT, "About", "")
        self.Bind(wx.EVT_MENU, self.onAbout, id=wx.ID_ABOUT)
        self.frame_menubar.Append(self.helpMenu, "&Help")
        self.SetMenuBar(self.frame_menubar)
        # Menu Bar end
        self.logText = wx.TextCtrl(self, wx.ID_ANY, "", style=wx.HSCROLL | wx.TE_MULTILINE | wx.TE_READONLY)
        self.button1 = wx.Button(self, wx.ID_ANY, "Press Me")

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_BUTTON, self.onPress, self.button1)
        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: CCFrame.__set_properties
        self.SetTitle("Control Client")
        self.button1.SetFocus()
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: CCFrame.__do_layout
        sizer_1 = wx.StaticBoxSizer(wx.StaticBox(self, wx.ID_ANY, "Logs"), wx.HORIZONTAL)
        sizer_1.Add(self.logText, 0, wx.EXPAND | wx.SHAPED, 0)
        sizer_1.Add(self.button1, 0, 0, 0)
        self.SetSizer(sizer_1)
        self.Layout()
        # end wxGlade

    def onDoThing(self, event):  # wxGlade: CCFrame.<event_handler>
        print("Event handler 'onDoThing' not implemented!")
        event.Skip()

    def onExit(self, event):  # wxGlade: CCFrame.<event_handler>
        print("Event handler 'onExit' not implemented!")
        event.Skip()

    def onHelp(self, event):  # wxGlade: CCFrame.<event_handler>
        print("Event handler 'onHelp' not implemented!")
        event.Skip()

    def onAbout(self, event):  # wxGlade: CCFrame.<event_handler>
        print("Event handler 'onAbout' not implemented!")
        event.Skip()

    def onPress(self, event):  # wxGlade: CCFrame.<event_handler>
        print("Event handler 'onPress' not implemented!")
        event.Skip()

# end of class CCFrame

class ControlClientApp(wx.App):
    def OnInit(self):
        self.ccframe = CCFrame(None, wx.ID_ANY, "")
        self.SetTopWindow(self.ccframe)
        self.ccframe.Show()
        return True

# end of class ControlClientApp

if __name__ == "__main__":
    ccApp = ControlClientApp(0)
    ccApp.MainLoop()
