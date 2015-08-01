# Eridani

This project is a standalone Jupyter environment for doing data science
using Python and Lua. It aims to include as many useful working libraries
and packages as possible, while remaining super easy to install and use.

This project uses git submodules for referencing other components:
    jupyter
    pyinstaller

## Building

Some requirements:
* NodeJS installed and working
* NPM installed and working properly (not as root)

To build everything:
* `make all`
This sets up virtualenv, installs requirements with pip, and calls
pyinstaller to build the application. Do `make test` to try it.


## Notes

I had some trouble with PIL libraries at one point, resolved by removing
the Ubuntu PIL packages. PyInstaller from pip is not enough up to date,
had to use the latest source and the latest zmq to get those working
properly.
