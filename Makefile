PYTHON := python2.7
VENV := venv
BUILD := build
DIST := dist

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

localtest: venv
	$(VENV)/bin/$(PYTHON) -m eridani eridani/TestNotebook.ipynb
localtestdebug: venv
	$(VENV)/bin/$(PYTHON) -m eridani eridani/TestNotebook.ipynb --debug
test: $(DIST)/server/server
	$(DIST)/server/server eridani/TestNotebook.ipynb
.PHONY:test localtest localtestdebug

clean:
	rm -fr $(BUILD) $(DIST)
.PHONY:clean

distclean: clean
	rm -fr $(VENV)
.PHONY:distclean
