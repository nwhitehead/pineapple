#!/usr/bin/env python

##################################
# Kernel imports (for client)
##################################

import numpy

##################################
# Browser
##################################

import sys
import subprocess
import wx
import wx.html2

class CustomBrowser(wx.Frame):
    def __init__(self, *args, **kwargs):
        wx.Frame.__init__(self, *args, **kwargs)
        self.children = []
        sizer = wx.BoxSizer(wx.VERTICAL)
        self.view = wx.html2.WebView.New(self)
        sizer.Add(self.view, 1, wx.EXPAND, 10)
        self.SetSizer(sizer)
        self.SetSize((700, 700))
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
        if txt[:4] == '$$$$':
            url = txt[4:]
            self.Spawn('http://localhost:8888' + url)
            return
        print('Title changed to {}'.format(event.GetString()))
    
    def LoadURL(self, url):
        self.view.LoadURL(url)

class BrowserShim():
    def __init__(self, app):
        # Needs app so we can stop server when last browser window closes
        self.app = app
    def open(self, url, **kwargs):
        print('BROWSER OPENING URL {}'.format(url))
        subprocess.call([sys.executable, '-m', 'eridani', 'viewurl', url])
        print('BROWSER RETURNED')
        self.app.stop()
