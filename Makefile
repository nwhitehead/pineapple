all:
	rm -fr build/ dist/
	venv/bin/pyinstaller server.spec -y
	mkdir dist/server/tcl dist/server/tk dist/server/notebook

reset:
	rm -fr venv/
	virtualenv venv
	venv/bin/pip install -r requirements.txt
	venv/bin/pip install -e pyinstaller
	venv/bin/pip install -e notebook

test:
	./dist/server/server
