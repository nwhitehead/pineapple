#!/usr/bin/env python

##################################
# Kernel imports (for client)
##################################

import numpy

##################################
# Browser
##################################

import subprocess

from PySide.QtCore import *
from PySide.QtGui import *
from PySide.QtWebKit import *
from PySide.QtNetwork import *

class Browser(QApplication):

    def __init__(self, url):
        print('open_url_mp PROCESS STARTING')
        QApplication.__init__(self, [])
        self.url = url
        # HACK to test websockets
        #self.url = "https://www.websocket.org/echo.html"
        self.initUI()

    def initUI(self):
        self.web = QWebView()
        self.web.settings().setAttribute(
            QWebSettings.WebAttribute.DeveloperExtrasEnabled, True)
        self.web.load(QUrl(self.url))
        self.web.show()

        self.inspector = QWebInspector()
        self.inspector.setPage(self.web.page())
        self.inspector.show()


class BrowserShim():
    def __init__(self, app):
        self.app = app
    def open(self, url, **kwargs):
        print('BROWSER OPENING URL {}'.format(url))
        subprocess.call([sys.executable, 'server.py', 'viewurl', url])
        print('BROWSER RETURNED')
        self.app.stop()

##################################
# Main
##################################

import sys
import threading

from notebook.notebookapp import NotebookApp
from ipykernel import kernelapp

def start_server():
    app = NotebookApp()
    app.initialize()
    app.start(BrowserShim(app))

if __name__ == '__main__':
    print('Hello from server.py')
    print('I was called with arguments {}'.format(sys.argv))
    if len(sys.argv) >=1 and sys.argv[1] == 'server.py':
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
        app = Browser(url)
        sys.exit(app.exec_())
    else:
        start_server()
