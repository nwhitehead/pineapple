##################################
# Main
##################################

import os
import sys
import threading
import wx

from notebook.notebookapp import NotebookApp
from ipykernel import kernelapp
from . import BrowserShim
from . import CustomBrowser

def start_server():
    app = NotebookApp()
    app.initialize()
    app.start(BrowserShim(app))

if __name__ == '__main__':
    # Allow hixie 76 protocol for WebSockets to account for Qt 4 (which PySide needs)
    os.environ['IPYTHON_ALLOW_DRAFT_WEBSOCKETS_FOR_PHANTOMJS'] = '1'
    print('Hello from server.py')
    print('I was called with arguments {}'.format(sys.argv))
    if len(sys.argv) >=2 and sys.argv[1] == '-m' and sys.argv[2] == 'eridani':
        del sys.argv[1]
        del sys.argv[1]
    print('FIXED: I was called with arguments {}'.format(sys.argv))
    if len(sys.argv) >= 2 and sys.argv[1] == '-m' and sys.argv[2] == 'ipykernel':
        print('I should make myself a Python kernel')
        print('Launching')
        kernelapp.launch_new_instance()
    elif len(sys.argv) >= 2 and sys.argv[1] == 'viewurl':
        url = sys.argv[2]
        print('I should view a URL {}'.format(url))
        print('Launching')
        app = wx.App(False)
        browser = CustomBrowser(None, -1)
        browser.LoadURL(url)
        browser.Show()
        sys.exit(app.MainLoop())
    else:
        start_server()
