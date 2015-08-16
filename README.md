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
* C++14 compiler (e.g. g++-4.9)
* Python, virtualenv (use brew on Mac OS X)
* NodeJS installed and working
* NPM installed and working properly (not as root)
* wxpython 2.9 or later (use brew on Mac OS X, binary wheel install on Windows, source compile on Linux)

WxPython should be system installed and working.
Test in python with "import wx; import wx.html2; wx.App()"

Once requirements are met, to build everything:
* `make all`
This sets up virtualenv, installs requirements with pip, and calls
pyinstaller to build the application. Do `make test` to try it.

## Linux

I am building wxpython from my fork of the git repo (no changes needed yet):
`git clone https://github.com/nwhitehead/wxWidgets.git`

Unbuntu packages needed:
`sudo apt-get install dpkg-dev build-essential python2.7-dev libjpeg-dev libtiff-dev libgtk2.0-dev libsdl1.2-dev libgstreamer-plugins-base0.10-dev`

Build:
`python build.py dox etg --nodoc sip build install`

## Mac OS X

Use Homebrew for python versions ([http://brew.sh/]). Follow instructions on website to
get get brew setup, then do:

* `brew install python python3 wxpython wxmac`

Next get virtualenv using pip:

* `pip install virtualenv`

I had trouble with the brew version of nodejs and npm. Instead go to [http://nodejs.org]
and directly install node, which also installs npm.

During the notebook build, the "Running setup.py develop for notebook" takes a long time, it
is actually running npm and installing many packages with no output.

## Notes

### JS

To rebuilt JavaScript/CSS static sources, from notebook directory do:
`python setup.py css js install`
