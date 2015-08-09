PYTHON := python2.7
VENV := venv
BUILD := build
DIST := dist
PLATFORM := $(shell uname)
DEMO_NOTEBOOK := demo/TestNotebook.ipynb

all: $(DIST)/eridani/eridani-ipykernel
.PHONY:all

$(DIST)/eridani-main/eridani-main: venv scripts/eridani-main scripts/eridani-main.spec
	rm -fr $(BUILD) $(DIST)
	$(VENV)/bin/pyinstaller scripts/eridani-main.spec -y
	mkdir -p $(DIST)/eridani-main/tcl $(DIST)/eridani-main/tk

venv: $(VENV)/bin/activate $(VENV)/bin/fwpython
.PHONE:venv

$(VENV)/bin/activate: requirements.txt
	test -d $(VENV) || virtualenv $(VENV)
	$(VENV)/bin/pip install -r requirements.txt
	touch $(VENV)/bin/activate

localtest: venv
	./scripts/eridani-main serve $(DEMO_NOTEBOOK)
localtestdebug: venv
	./scripts/eridani-main serve $(DEMO_NOTEBOOK) --debug
test: $(DIST)/eridani-main/eridani-main
	$(DIST)/eridani-main/eridani-main $(DEMO_NOTEBOOK)
.PHONY:test localtest localtestdebug

clean:
	rm -fr $(BUILD) $(DIST)
.PHONY:clean

distclean: clean
	rm -fr $(VENV)
.PHONY:distclean
