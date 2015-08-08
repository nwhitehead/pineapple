PYTHON := python2.7
VENV := venv
DIST_PACKAGES := /usr/lib/$(PYTHON)/dist-packages
LOCAL_PACKAGES := $(VENV)/lib/$(PYTHON)/site-packages
WX_PACKAGE := wx-2.8-gtk2-unicode
BUILD := build
DIST := dist

all: $(DIST)/server/server
.PHONY:all

$(DIST)/server/server: venv server.spec server.py
	rm -fr $(BUILD) $(DIST)
	$(VENV)/bin/pyinstaller server.spec -y
	mkdir $(DIST)/server/tcl $(DIST)/server/tk

venv: $(VENV)/bin/activate $(VENV)/wxwidgets
.PHONE:venv

$(VENV)/bin/activate: requirements.txt
	test -d $(VENV) || virtualenv $(VENV)
	$(VENV)/bin/pip install -r requirements.txt
	touch $(VENV)/bin/activate

$(VENV)/wxwidgets:
	# Link in distribution python-wxwidgets into virtualenv space
	rm -f $(LOCAL_PACKAGES)/wx.pth
	ln -s $(DIST_PACKAGES)/wx $(LOCAL_PACKAGES)/wx.pth
	rm -f $(LOCAL_PACKAGES)/wxversion.py
	ln -s $(DIST_PACKAGES)/wxversion.py $(LOCAL_PACKAGES)/wxversion.py
	rm -f $(LOCAL_PACKAGES)/$(WX_PACKAGE)
	ln -s $(DIST_PACKAGES)/$(WX_PACKAGE) $(LOCAL_PACKAGES)/$(WX_PACKAGE)
	touch $(VENV)/wxwidgets

test: $(DIST)/server/server
	$(DIST)/server/server TestNotebook.ipynb
.PHONY:test

clean:
	rm -fr $(BUILD) $(DIST)
.PHONY:clean

distclean: clean
	rm -fr $(VENV)
.PHONY:distclean
