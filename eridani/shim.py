
import subprocess
import sys

class BrowserShim():
    def __init__(self, app):
        # Needs app so we can stop server when last browser window closes
        self.app = app
    def open(self, url, **kwargs):
        # Use sys.executable to get python/compiled executable
        subprocess.call([sys.executable, '-m', 'eridani', 'viewurl', url])
        self.app.stop()
