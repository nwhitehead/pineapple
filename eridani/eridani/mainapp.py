##################################
# Main
##################################

import os
import sys
import threading
import wx

from notebook.notebookapp import NotebookApp
from ipykernel import kernelapp
from .shim import BrowserShim
from .browser import CustomBrowser

def main():
    if len(sys.argv) >=2 and sys.argv[1] == '-m' and sys.argv[2] == 'eridani':
        del sys.argv[1]
        del sys.argv[1]
    if len(sys.argv) >= 2 and sys.argv[1] == '-m' and sys.argv[2] == 'ipykernel':
        kernelapp.launch_new_instance()
    elif len(sys.argv) >= 2 and sys.argv[1] == 'viewurl':
        url = sys.argv[2]
        app = wx.App(False)
        browser = CustomBrowser(None, -1)
        browser.LoadURL(url)
        browser.Show()
        sys.exit(app.MainLoop())
    else:
        app = NotebookApp()
        app.initialize()
        app.start(BrowserShim(app))

if __name__ == '__main__':
    main()
