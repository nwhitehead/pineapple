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

class CustomWebView(QWebView):
    def __init__(self, parent=None):
        super(CustomWebView, self).__init__(parent)
    def createWindow(self, typ):
        print('createWindow {}'.format(typ))
        self.window = QWebView()
        self.webpage = QWebPage(self.window)
        self.window.setPage(self.webpage)
        self.window.show()
        return self.window

class Browser(QApplication):

    def __init__(self, url):
        print('open_url_mp PROCESS STARTING')
        QApplication.__init__(self, [])
        self.url = url
        # HACK to test websockets
        #self.url = "https://www.websocket.org/echo.html"
        self.initUI()

    def about(self):
        QMessageBox.about(self, "About app", "MESSAGE")

    def initUI(self):
        self.view = QWidget()

        self.layout = QVBoxLayout()

        self.locationBar = QLineEdit()
        self.layout.addWidget(self.locationBar)

        self.web = CustomWebView()
        self.web.settings().setAttribute(
            QWebSettings.WebAttribute.DeveloperExtrasEnabled, True)
        self.web.load(QUrl(self.url))
        self.layout.addWidget(self.web)

        self.view.setLayout(self.layout)
        self.view.show()


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

import os
import sys
import threading

from notebook.notebookapp import NotebookApp
from ipykernel import kernelapp

def start_server():
    app = NotebookApp()
    app.initialize()
    app.start(BrowserShim(app))

if __name__ == '__main__':
    # Allow hixie 76 protocol for WebSockets to account for Qt 4 (which PySide needs)
    os.environ['IPYTHON_ALLOW_DRAFT_WEBSOCKETS_FOR_PHANTOMJS'] = '1'
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
