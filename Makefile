PYTHON := python2.7
VENV := venv
BUILD := build
DIST := dist
PLATFORM := $(shell uname)

LOCAL_PACKAGES := $(VENV)/lib/$(PYTHON)/site-packages
ifeq ($(PLATFORM),Linux)
	## Linux
	DIST_PACKAGES := /usr/lib/$(PYTHON)/dist-packages
	WX_PACKAGE := wx-3.0-gtk2-unicode
else
	ifeq ($(PLATFORM),Darwin)
		## Mac
		DIST_PACKAGES := /usr/local/Cellar/wxpython/3.0.2.0/lib/$(PYTHON)/site-packages
		WX_PACKAGE := wx-3.0-osx_cocoa
	endif
endif

all: $(DIST)/server/server
.PHONY:all

$(DIST)/server/server: venv server.spec server.py
	rm -fr $(BUILD) $(DIST)
	$(VENV)/bin/pyinstaller server.spec -y
	mkdir $(DIST)/server/tcl $(DIST)/server/tk

venv: $(VENV)/bin/activate
.PHONE:venv

$(VENV)/bin/activate: requirements.txt
	test -d $(VENV) || virtualenv $(VENV) --system-site-packages
	$(VENV)/bin/pip install -r requirements.txt
	touch $(VENV)/bin/activate

$(VENV)/bin/fwpython: tools/fwpython
	cp -f tools/fwpython $(VENV)/bin/fwpython

$(VENV)/wxwidgets:
	# Link in distribution python-wxwidgets into virtualenv space
	rm -f $(LOCAL_PACKAGES)/wx.pth
	ln -s $(DIST_PACKAGES)/wx.pth $(LOCAL_PACKAGES)/wx.pth
	rm -f $(LOCAL_PACKAGES)/wxversion.py
	ln -s $(DIST_PACKAGES)/wxversion.py $(LOCAL_PACKAGES)/wxversion.py
	rm -f $(LOCAL_PACKAGES)/$(WX_PACKAGE)
	ln -s $(DIST_PACKAGES)/$(WX_PACKAGE) $(LOCAL_PACKAGES)/$(WX_PACKAGE)
	touch $(VENV)/wxwidgets

localtest: venv
	$(VENV)/bin/fwpython -m eridani eridani/TestNotebook.ipynb
localtestdebug: venv
	$(VENV)/bin/fwpython -m eridani eridani/TestNotebook.ipynb --debug
test: $(DIST)/server/server
	$(DIST)/server/server eridani/TestNotebook.ipynb
.PHONY:test localtest localtestdebug

clean:
	rm -fr $(BUILD) $(DIST)
.PHONY:clean

distclean: clean
	rm -fr $(VENV)
.PHONY:distclean
