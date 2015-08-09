PYTHON := python2.7
VENV := venv
BUILD := build
DIST := dist
PLATFORM := $(shell uname)
DEMO_NOTEBOOK := demo/TestNotebook.ipynb

LOCAL_PACKAGES := $(VENV)/lib/$(PYTHON)/site-packages
ifeq ($(PLATFORM),Darwin)
	## Mac
	DIST_PACKAGES := /usr/local/Cellar/wxpython/3.0.2.0/lib/$(PYTHON)/site-packages
	WX_PACKAGE := wx-3.0-osx_cocoa
endif

all: $(DIST)/server/server
.PHONY:all

$(DIST)/eridani/eridani: venv eridani/eridani.spec
	rm -fr $(BUILD) $(DIST)
	$(VENV)/bin/pyinstaller eridani/eridani.spec -y
	mkdir $(DIST)/server/tcl $(DIST)/server/tk

venv: $(VENV)/bin/activate $(VENV)/bin/fwpython
.PHONE:venv

$(VENV)/bin/activate: requirements.txt
	test -d $(VENV) || virtualenv $(VENV) --system-site-packages
	$(VENV)/bin/pip install -r requirements.txt
	touch $(VENV)/bin/activate

$(VENV)/bin/fwpython: tools/fwpython
ifeq ($(PLATFORM),Darwin)
	cp -f tools/fwpython $(VENV)/bin/fwpython
else
	cp -f $(VENV)/bin/python $(VENV)/bin/fwpython
endif

$(VENV)/wxwidgets:
	# Link in distribution python-wxwidgets into virtualenv space
	ifeq ($(PLATFORM),Darwin)
		rm -f $(LOCAL_PACKAGES)/wx.pth
		ln -s $(DIST_PACKAGES)/wx.pth $(LOCAL_PACKAGES)/wx.pth
		rm -f $(LOCAL_PACKAGES)/wxversion.py
		ln -s $(DIST_PACKAGES)/wxversion.py $(LOCAL_PACKAGES)/wxversion.py
		rm -f $(LOCAL_PACKAGES)/$(WX_PACKAGE)
		ln -s $(DIST_PACKAGES)/$(WX_PACKAGE) $(LOCAL_PACKAGES)/$(WX_PACKAGE)
	endif
	touch $(VENV)/wxwidgets

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
