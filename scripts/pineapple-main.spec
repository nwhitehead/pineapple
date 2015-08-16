# -*- mode: python -*-

block_cipher = None

a = Analysis(['scripts/eridani-main'],
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
pyz = PYZ(a.pure,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=True,
          name='eridani-main',
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
               strip=None,
               upx=True,
               name='eridani-main')
