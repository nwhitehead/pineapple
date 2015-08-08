import wx
import wx.html2

class MyBrowser(wx.Dialog):
    def __init__(self, *args, **kwargs):
        wx.Dialog.__init__(self, *args, **kwargs)
        sizer = wx.BoxSizer(wx.VERTICAL)
        self.view = wx.html2.WebView.New(self)
        sizer.Add(self.view, 1, wx.EXPAND, 10)
        self.SetSizer(sizer)
        self.SetSize((700, 700))
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

    def OnCloseWindow(self, event):
        self.Destroy()

if __name__ == '__main__':
    app = wx.App(False)
    browser = MyBrowser(None, -1)
    browser.view.LoadURL("http://www.google.com")
    browser.Show()
    app.MainLoop()
