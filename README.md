# Pineapple

This project is a standalone Jupyter environment for doing data science
using Python. It aims to include many useful working libraries
and packages, while remaining super easy to install and use.

## Building Prerequisites

General requirements:
* C++11 compiler (e.g. g++-4.9 or later)
* wxWidgets 3.x (source compile)
* lessc (for compiling .less files, get it with npm)

### Mac OS X

For wxWidgets, I downloaded the source then used:

```
mkdir build-release
cd build-release
../configure --enable-shared --enable-monolithic --with-osx_cocoa CXX='clang++ -std=c++11 -stdlib=libc++' CC=clang --with-macosx-version-min=10.8 --disable-debug --without-liblzma
make -j4
sudo make install
```

### Ubuntu 14.04

To get g++-4.9 (required) use the Ubuntu Toolchain PPA.

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install g++-4.9
```

You'll need various development files to compile.

```
sudo apt-get install build-essential libz-dev libgtk2.0-dev
    libreadline-dev libssl-dev libncursesw5-dev
    libgdbm-dev libsqlite3-dev libbz2-dev liblzma-dev
    libreadline-gplv2-dev
    libc6-dev
```

Get the wxWidget source and compile:

```
mkdir buildgtk
cd buildgtk
../configure --with-gtk
make -j4
sudo make install
```

## Building for local testing

If prerequisites are met, you should be able to do:

```
mkdir build
cd build
cmake ..
make
```

This builds python and various libraries and builds
the main application.

```
make custom-install
```
This installs pip requirements and then copies custom
files into the notebook static directory. Now you can
try the local application with `make local-test`.

## Distribution

Redistributable packages are built using CPack.

```
make install
make package
```

The final redistributable files will be placed at the top level of the
build directory. Final packages will be compressed tar files for
Linux, DMG images for Mac.

## Notes

### OS X

If you want to change the icon images, you'll need to regenerate the icns files.
I did this using [https://iconverticons.com] because the command-line tools
I found were out of date and I didn't want to hassle with automating it.

## Contact

Pineapple is a project of Nathan Whitehead, copyright 2015.
Let me know what you think at nwhitehe [at] gmail.com.
