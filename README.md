# Eridani

This project is a standalone Jupyter environment for doing data science
using Python and Lua. It aims to include as many useful working libraries
and packages as possible, while remaining super easy to install and use.

This project uses git submodules for referencing other components:
    pyinstaller

You can get them when you clone with `git clone --recursive ...`. If you
didn't do that when you cloned, you can get them afterwards with
`git submodule update --init --recursive` from within the repo.

## Building Prerequisites

General requirements:
* C++11 compiler (e.g. g++-4.9)
* Python, virtualenv (use brew on Mac OS X)
* wxWidgets 3.x (source compile)

### Mac OS X

Use Homebrew for python versions ([http://brew.sh/]). Follow instructions
on website to get get brew setup, then do:

```
brew install python
```

Next get virtualenv using pip (pip is installed as part of Python).

```
pip install virtualenv
```

For wxWidgets, I downloaded the source then used:

```
mkdir build-release
cd build-release
../configure --enable-shared --enable-monolithic --with-osx_cocoa CXX='clang++ -std=c++11 -stdlib=libc++' CC=clang --with-macosx-version-min=10.8 --disable-debug
make -j4
sudo make install
```

### Ubuntu 14.04

To get g++-4.9 use the Ubuntu Toolchain PPA.

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install g++-4.9
```

You'll need various development files to compile.

```
sudo apt-get install build-essential libgtk2.0-dev python2.7
```

Get the wxWidget source and compile:

```
mkdir buildgtk
cd buildgtk
../configure --with-gtk
make -j4
sudo make install
```

Make sure pip and virtualenv are updated.

```
pip install --upgrade pip
pip install --upgrade virtualenv
```

## Building for local testing

If prerequisites are met, you should be able to do:

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=. ..
make
```

This sets up virtualenv, installs requirements with pip, and builds
the main application. Do `make localtest` to try it.

## Distribution

To build a redistributable package, you need to first package the Jupyter
server component using PyInstaller.

```
make server
```

Next you need to create the redistributable application in the build
directory.

```
make install
```

Finally you package up the application into a platform-specific format
for distribution using CPack.

```
make package
```

The final redistributable files will be placed at the top level of the
build directory.

## Notes

### OS X

If you want to change the icon images, you'll need to regenerate the icns files.
I did this using [https://iconverticons.com] because the command-line tools
I found were out of date and I didn't want to hassle with automating it.
