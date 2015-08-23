# -*- mode: python -*-

block_cipher = None

a = Analysis(['scripts/pineapple-server'],
             pathex=['.'],
             hiddenimports=[
                'IPython.extensions.storemagic',
                'notebook.tree.handlers',
                'notebook.files.handlers',
                'notebook.notebook.handlers',
                'notebook.nbconvert.handlers',
                'notebook.kernelspecs.handlers',
                'notebook.edit.handlers',
                'notebook.services.api.handlers',
                'notebook.services.config.handlers',
                'notebook.services.kernels.handlers',
                'notebook.services.contents.handlers',
                'notebook.services.sessions.handlers',
                'notebook.services.nbconvert.handlers',
                'notebook.services.kernelspecs.handlers',
                'notebook.services.security.handlers',
             ],
             hookspath=None,
             runtime_hooks=None,
             excludes=None,
             cipher=block_cipher)

for ind in reversed(range(len(a.datas))):
    if a.datas[ind][0].startswith('..'):
        olddata = a.datas.pop(ind)
        print 'Removing %s from a.datas'%(olddata[0])

pyz = PYZ(a.pure,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=True,
          name='pineapple-server',
          debug=False,
          strip=None,
          upx=True,
          console=True )
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               Tree('venv/lib/python2.7/site-packages/notebook/static', prefix='notebook/static'),
               Tree('venv/lib/python2.7/site-packages/notebook/templates', prefix='notebook/templates'),
               Tree('venv/lib/python2.7/site-packages/nbformat/v3', prefix='nbformat/v3'),
               Tree('venv/lib/python2.7/site-packages/nbformat/v4', prefix='nbformat/v4'),
               Tree('venv/src/nbconvert/nbconvert/templates', prefix='nbconvert/templates'),
               Tree('venv/src/nbconvert/nbconvert/resources', prefix='nbconvert/resources'),
               strip=None,
               upx=True,
               name='pineapple-server')
