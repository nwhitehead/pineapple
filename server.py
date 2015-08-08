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
    def __init__(self, parent=None, url=None):
        super(CustomWebView, self).__init__(parent)
        self.children = []
        self.setZoomFactor(1.1)
        self.settings().setAttribute(
            QWebSettings.WebAttribute.DeveloperExtrasEnabled, True)
        if url is not None:
            self.load(QUrl(url))

    def createWindow(self, typ):
        print('createWindow {}'.format(typ))
        child = Browser()
        webpage = QWebPage(child)
        child.view.setPage(webpage)
        child.show()
        self.children.append(child)
        return child.view

class Browser(QWidget):

    def __init__(self, parent=None, url=None):
        super(Browser, self).__init__(parent)
        self.resize(600, 600)
        self.layout = QVBoxLayout(self)
        self.setLayout(self.layout)

        self.locationBar = QLineEdit()
        self.layout.addWidget(self.locationBar)

        self.view = CustomWebView(parent=self, url=url)
        self.layout.addWidget(self.view)


class BrowserApp(QApplication):

    def __init__(self, url=None):
        print('open_url_mp PROCESS STARTING')
        QApplication.__init__(self, [])
        self.url = url
        ## Set self.url manually here for testing if you want
        self.view = Browser(url=self.url)
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
        app = BrowserApp(url)
        sys.exit(app.exec_())
    else:
        start_server()
