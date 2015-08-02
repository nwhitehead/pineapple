all: dist/server/server
.PHONY:all

dist/server/server: venv server.spec server.py
	rm -fr build/ dist/
	venv/bin/pyinstaller server.spec -y
	mkdir dist/server/tcl dist/server/tk

venv: venv/bin/activate
venv/bin/activate: requirements.txt
	test -d venv || virtualenv venv
	venv/bin/pip install -r requirements.txt
	touch venv/bin/activate

test: dist/server/server
	./dist/server/server TestNotebook.ipynb
.PHONY:test

clean:
	rm -fr build/ dist/
.PHONY:clean

distclean: clean
	rm -fr venv/
.PHONY:distclean
