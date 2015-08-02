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

def open_url(url):
    print('open_url_mp PROCESS STARTING')

    def _request_finished(reply):
        print('LOADED {}'.format(reply.url()))
    network_manager = QNetworkAccessManager()
    network_manager.finished.connect(_request_finished)

    app = QApplication([])

    page = QWebPage()
    page.setNetworkAccessManager(network_manager)
    page.mainFrame().setUrl(url)

    browser = QWebView()
    browser.setPage(page)
    browser.show()
    retval = app.exec_()
    print('RETURNED {}'.format(retval))
    sys.exit(retval)

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
        open_url(url)
    else:
        start_server()
