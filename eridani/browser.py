##################################
# Browser
##################################

import sys
import subprocess
import wx
import wx.html2

default_size = (700, 700)
title_protocol_new_window = '$$$$'

class CustomBrowser(wx.Frame):
    def __init__(self, *args, **kwargs):
        wx.Frame.__init__(self, *args, **kwargs)
        self.children = []
        sizer = wx.BoxSizer(wx.VERTICAL)
        self.view = wx.html2.WebView.New(self)
        sizer.Add(self.view, 1, wx.EXPAND, 10)
        self.SetSizer(sizer)
        self.SetSize(default_size)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)
        self.Bind(wx.html2.EVT_WEBVIEW_ERROR, self.OnError)
        self.Bind(wx.html2.EVT_WEBVIEW_LOADED, self.OnLoaded, self.view)
        self.Bind(wx.html2.EVT_WEBVIEW_NAVIGATING, self.OnNavigate, self.view)
        self.Bind(wx.html2.EVT_WEBVIEW_NEWWINDOW, self.OnNewWindow)
        self.Bind(wx.html2.EVT_WEBVIEW_TITLE_CHANGED, self.OnTitleChanged, self.view)

    def Spawn(self, url):
        child = CustomBrowser(None, -1)
        child.LoadURL(url)
        child.Show()
        self.children.append(child)

    def OnCloseWindow(self, event):
        self.Destroy()

    def OnNavigate(self, event):
        print('Navigate [{}][{}]'.format(event.GetTarget(), event.GetURL()))

    def OnLoaded(self, event):
        print('Loaded {}{}'.format(event.GetTarget(), event.GetURL()))

    def OnError(self, event):
        print('WebView error')

    def OnNewWindow(self, event):
        url = event.GetURL()
        print('New window requested [{}]'.format(url))
        self.Spawn(url)

    def OnTitleChanged(self, event):
        txt = event.GetString()
        if txt.startswith(title_protocol_new_window):
            # Backdoor communication mechanism to open a new window
            url = txt[len(title_protocol_new_window):]
            self.Spawn('http://localhost:8888' + url)
            return
        print('Title changed to {}'.format(event.GetString()))
    
    def LoadURL(self, url):
        self.view.LoadURL(url)
