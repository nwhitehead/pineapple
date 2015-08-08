# Eridani

This project is a standalone Jupyter environment for doing data science
using Python and Lua. It aims to include as many useful working libraries
and packages as possible, while remaining super easy to install and use.

This project uses git submodules for referencing other components:
    jupyter
    pyinstaller

You can get them when you clone with `git clone --recursive ...`. If you
didn't do that when you cloned, you can get them afterwards with
`git submodule update --init --recursive` from within the repo.

## Building

Some misc. requirements:
* Python, virtualenv (use brew on Mac OS X)
* NodeJS installed and working
* NPM installed and working properly (not as root)

To get newest version of wxWidgets, need new versions of pip
and distutils:
`pip install --upgrade pip distutils`

* wxWidgets

To build everything:
* `make all`
This sets up virtualenv, installs requirements with pip, and calls
pyinstaller to build the application. Do `make test` to try it.

BUG: The PySide installation process sometimes does not do the postinstall,
you need to run `venv/bin/python venv/bin/pyside_postinstall.py -install`
manually to finish installation.

## Linux

For Ubuntu building:
`sudo apt-get install dpkg-dev build-essential python2.7-dev libjpeg-dev libtiff-dev libgtk2.0-dev libsdl1.2-dev libgstreamer-plugins-base0.10-dev`

CEFPython3 needs to be downloaded from:
https://www.dropbox.com/sh/zar95p27yznuiv1/AAAL7G5cwnGTeS9nFmFZO21Va/31.2?dl=0

## Mac OS X

Use Homebrew for python versions ([http://brew.sh/]). Follow instructions on website to
get get brew setup, then do:

* `brew install python python3 qt`

Next get virtualenv using pip:

* `pip install virtualenv`

I had trouble with the brew version of nodejs and npm. Instead go to [http://nodejs.org]
and directly install node, which also installs npm.

During the build, the "Running setup.py develop for notebook" takes a long time, it
is actually running npm and installing many packages with no output.

Alternative for Qt: Get Qt directly from [http://www.qt.io/download-open-source/#section-2]

NEW: CEF Python 3 requires "pip install cefpython3" on Mac.

## Notes

I had some trouble with PIL libraries at one point, resolved by removing
the Ubuntu PIL packages. PyInstaller from pip is not enough up to date,
had to use the latest source and the latest zmq to get those working
properly.

