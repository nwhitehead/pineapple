PYTHON := python2.7
VENV := venv
BUILD := build
DIST := dist
PLATFORM := $(shell uname)
DEMO_NOTEBOOK := demo/TestNotebook.ipynb

LOCAL_PACKAGES := $(VENV)/lib/$(PYTHON)/site-packages
ifeq ($(PLATFORM),Linux)
	## Linux
	DIST_PACKAGES := /usr/local/lib/$(PYTHON)/dist-packages
	WX_PACKAGE := wx
else
	ifeq ($(PLATFORM),Darwin)
		## Mac
		DIST_PACKAGES := /usr/local/Cellar/wxpython/3.0.2.0/lib/$(PYTHON)/site-packages
		WX_PACKAGE := wx-3.0-osx_cocoa
	endif
endif

all: $(DIST)/server/server
.PHONY:all

$(DIST)/eridani/eridani: venv eridani/eridani.spec
	rm -fr $(BUILD) $(DIST)
	$(VENV)/bin/pyinstaller eridani/eridani.spec -y
	mkdir $(DIST)/eridani/tcl $(DIST)/eridani/tk

venv: $(VENV)/bin/activate $(VENV)/bin/fwpython
.PHONE:venv

$(VENV)/bin/activate: requirements.txt
	test -d $(VENV) || virtualenv $(VENV)
	$(VENV)/bin/pip install -r requirements.txt
	touch $(VENV)/bin/activate

$(VENV)/bin/fwpython: tools/fwpython
ifeq ($(PLATFORM),Darwin)
	cp -f tools/fwpython $(VENV)/bin/fwpython
else
	cp -f $(VENV)/bin/python $(VENV)/bin/fwpython
endif

# Link in distribution python-wxwidgets into virtualenv space
ifeq ($(PLATFORM),Linux)
$(VENV)/wxwidgets:
		rm -fr $(LOCAL_PACKAGES)/wx
		ln -s $(DIST_PACKAGES)/wx $(LOCAL_PACKAGES)/wx
		touch $(VENV)/wxwidgets
else
	ifeq ($(PLATFORM),Darwin)
$(VENV)/wxwidgets:
		rm -f $(LOCAL_PACKAGES)/wx.pth
		ln -s $(DIST_PACKAGES)/wx.pth $(LOCAL_PACKAGES)/wx.pth
		rm -f $(LOCAL_PACKAGES)/wxversion.py
		ln -s $(DIST_PACKAGES)/wxversion.py $(LOCAL_PACKAGES)/wxversion.py
		rm -f $(LOCAL_PACKAGES)/$(WX_PACKAGE)
		ln -s $(DIST_PACKAGES)/$(WX_PACKAGE) $(LOCAL_PACKAGES)/$(WX_PACKAGE)
		touch $(VENV)/wxwidgets
	endif
endif

localtest: venv
	$(VENV)/bin/fwpython -m eridani $(DEMO_NOTEBOOK)
localtestdebug: venv
	$(VENV)/bin/fwpython -m eridani $(DEMO_NOTEBOOK) --debug
test: $(DIST)/eridani/eridani
	$(DIST)/eridani/eridani $(DEMO_NOTEBOOK)
.PHONY:test localtest localtestdebug

clean:
	rm -fr $(BUILD) $(DIST)
.PHONY:clean

distclean: clean
	rm -fr $(VENV)
.PHONY:distclean
