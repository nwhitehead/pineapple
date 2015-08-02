import sys

from PySide.QtCore import *
from PySide.QtGui import *
from PySide.QtWebKit import *

if __name__ == '__main__':
    app = QApplication(sys.argv)
    browser = QWebView()
    browser.setUrl(sys.argv[1])
    browser.show()
    sys.exit(app.exec_())
