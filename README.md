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

Some requirements:
* Python, virtualenv (use brew on Mac OS X)
* NodeJS installed and working
* NPM installed and working properly (not as root)

To build everything:
* `make all`
This sets up virtualenv, installs requirements with pip, and calls
pyinstaller to build the application. Do `make test` to try it.

## Mac OS X

Use Homebrew for tools (http://brew.sh/). Follow instructions on website to
get get brew setup, then do:
* `brew install python python3 virtualenv`

I had trouble with the brew version of nodejs and npm. Instead go to nodejs.org
and directly install node, which also installs npm.


## Notes

I had some trouble with PIL libraries at one point, resolved by removing
the Ubuntu PIL packages. PyInstaller from pip is not enough up to date,
had to use the latest source and the latest zmq to get those working
properly.

