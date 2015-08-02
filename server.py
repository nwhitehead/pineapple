#!/usr/bin/env python

##################################
# Kernel imports (for client)
##################################

import numpy

##################################
# Browser
##################################

import multiprocessing

from PySide.QtCore import *
from PySide.QtGui import *
from PySide.QtWebKit import *

def open_url_mp(url):
    print('open_url_mp PROCESS STARTING')
    app = QApplication([])
    browser = QWebView()
    browser.setUrl('http://localhost:8888')
    browser.show()
    retval = app.exec_()
    print('RETURNED {}'.format(retval))
    return retval

class BrowserShim():
    def __init__(self):
        pass
    def open(self, url, **kwargs):
        print('BROWSER OPENING URL {}'.format(url))
        b = lambda : open_url_mp(url)
        multiprocessing.Process(target=b).start()

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
    app.start(BrowserShim())

if __name__ == '__main__':
    print('Hello from server.py')
    print('I was called with arguments {}'.format(sys.argv))
    if len(sys.argv) >= 2 and sys.argv[1] == '-m' and sys.argv[2] == 'ipykernel':
        print('I should make myself a Python kernel')
        print('Launching')
        kernelapp.launch_new_instance()
    else:
        start_server()
