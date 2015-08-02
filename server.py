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

class Browser(object):

    def __init__(self, url):
        print('open_url_mp PROCESS STARTING')
        self.network_manager = QNetworkAccessManager()
        self.network_manager.createRequest = self._my_create_request
        self.network_manager.finished.connect(self._request_finished)

        self.app = QApplication([])

        self.page = QWebPage()
        self.page.setNetworkAccessManager(self.network_manager)
        self.page.mainFrame().setUrl(url)

        self.view = QWebView()
        self.view.setPage(self.page)
        self.view.show()

        self.inspector = QWebInspector()
        self.inspector.setPage(self.page)

    def start(self):
        return self.app.exec_()

    def _request_finished(self, reply):
        print('LOADED {}'.format(reply.url()))

    def _my_make_request(self, url):
        print('REQUEST {}'.format(url))
        request = QNetworkRequest()
        request.setUrl(QUrl(url))
        return request

    def _my_create_request(self, operation, request, data):
        print('REQUEST {}'.format(request.url()))
        if data:
            print(data.readAll())
        reply = QNetworkAccessManager.createRequest(self.network_manager,
            operation, request, data)
        return reply

    def _urlencode_post_data(self, data):
        print('ENCODING')
        params = QUrl()
        for (key, value) in data.items():
            params.addQueryItem(key, unicode(value))
        return params.encodedQuery()


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
        sys.exit(Browser(url).start())
    else:
        start_server()
