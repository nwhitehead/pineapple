all: dist/server/server
.PHONY:all

dist/server/server: venv server.spec server.py
	rm -fr build/ dist/
	venv/bin/pyinstaller server.spec -y
	mkdir dist/server/tcl dist/server/tk

venv: venv/bin/activate venv/wxwidgets
.PHONE:venv

venv/bin/activate: requirements.txt
	test -d venv || virtualenv venv
	venv/bin/pip install -r requirements.txt
	touch venv/bin/activate

venv/wxwidgets:
	# Link in distribution python-wxwidgets into virtualenv space
	rm -f venv/lib/python2.7/site-packages/wx.pth
	ln -s /usr/lib/python2.7/dist-packages/wx venv/lib/python2.7/site-packages/wx.pth
	rm -f venv/lib/python2.7/site-packages/wxversion.py
	ln -s /usr/lib/python2.7/dist-packages/wxversion.py venv/lib/python2.7/site-packages/wxversion.py
	rm -f venv/lib/python2.7/site-packages/wx-2.8-gtk2-unicode
	ln -s /usr/lib/python2.7/dist-packages/wx-2.8-gtk2-unicode venv/lib/python2.7/site-packages/wx-2.8-gtk2-unicode
	touch venv/wxwidgets

test: dist/server/server
	./dist/server/server TestNotebook.ipynb
.PHONY:test

clean:
	rm -fr build/ dist/
.PHONY:clean

distclean: clean
	rm -fr venv/
.PHONY:distclean
