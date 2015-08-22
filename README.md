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
* wxwidgets 3.x (used brew on Mac, source compile on Linux)

Once requirements are met, to build everything:
* `make all`
This sets up virtualenv, installs requirements with pip, and calls
pyinstaller to build the application. Do `make localtest` to try it.

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

### OS X

If you want to change the icon images, you'll need to regenerate the icns files.
I did this using [https://iconverticons.com] because the command-line tools
I found were out of date and I didn't want to hassle with automating it.

## Development Builds

It can be convenient to build and run locally. To do this, configure CMake
to use the build directory as the install location.

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=. ..
make install
make localtest
```
