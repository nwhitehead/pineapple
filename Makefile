all:
	rm -fr build/ dist/
	pyinstaller server.spec -y
	mkdir dist/server/tcl dist/server/tk dist/server/notebook

test:
	./dist/server/server
