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
* Setup virtualenv (`virtualenv venv`)
* Active virtual env (`source venv/bin/active`)
* Install packages using pip (`pip install -r requirements.txt`)
* Install pyinstaller (`cd pyinstaller && pip install -e .`)
* Install jupyter (`cd notebook && pip install -e .`)

Now test it out locally to make sure it works.
* `python server.py`


## Notes

get jupyter from git
    https://github.com/jupyter/notebook

setup venv
    virtualenv venv

install package
    pip install --pre -e .

had to install terminado
    pip install terminado

get latest pyinstaller
    pip install pyinstaller

pyside???
    http://pyside.readthedocs.org/en/latest/building/linux.html
    normal build, (wheel compile, then pip install wheel)
    needed to manually run python venv/bin/pyside_postinstall.py -install

pyinstaller -d -y test.py --paths=venv/lib/python2.7/site-packages/  && ./dist/test/test 
pyinstaller -y jupyter-notebook.spec  && ./dist/jupyter-notebook/jupyter-notebook

Works with latest PyInstaller:
    PyInstaller (2.1.1dev-9d0e0ad)
    https://github.com/pyinstaller/pyinstaller.git
 
Remove PIL ubuntu packages, pip install Pillow in venv, Removed Pillow (no image support now?)
Remove python-zmq ubuntu package, pip install zmq, pip install cython

rm -fr build/ dist/ && pyinstaller jupyter-notebook.spec -y && mkdir dist/jupyter-notebook/tcl dist/jupyter-notebook/tk dist/jupyter-notebook/notebook && ./dist/jupyter-notebook/jupyter-notebook 

Need to make empty dirs for:
    tcl/
    tk/
    notebook/

No need to tweak spec file except for some hidden imports:
    notebook.tree.handlers

