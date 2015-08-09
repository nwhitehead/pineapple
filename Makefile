PYTHON := python2.7
VENV := venv
BUILD := build
DIST := dist
PLATFORM := $(shell uname)
DEMO_NOTEBOOK := demo/TestNotebook.ipynb

all: $(DIST)/eridani/eridani-ipykernel
.PHONY:all

$(DIST)/eridani/eridani-ipykernel: venv scripts/eridani-ipykernel scripts/eridani-ipykernel.spec
	rm -fr $(BUILD) $(DIST)
	$(VENV)/bin/pyinstaller scripts/eridani-ipykernel.spec -y
	mkdir -p $(DIST)/eridani-ipykernel/tcl $(DIST)/eridani-ipykernel/tk

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
$(VENV)/wxwidgets:
ifeq ($(PLATFORM),Linux)
		rm -fr $(LOCAL_PACKAGES)/wx
		ln -s $(DIST_PACKAGES)/wx $(LOCAL_PACKAGES)/wx
		touch $(VENV)/wxwidgets
else
ifeq ($(PLATFORM),Darwin)
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
