import os

if __name__ == '__main__':
    filename = 'data/images/IconsShadow.svg'
    x0 = 34.4
    y0 = 934.2
    w = 96.0
    xspacing = 131.8
    yspacing = -131.8
    res = 256
    outpath = 'data/images'

    x = x0
    y = y0
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Save.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'SaveGreen.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'SaveYellow.png'), res, x, y, x+w, y+w))

    x = x0
    y = y0 + yspacing * 1
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Plus.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Delete.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Synchronize.png'), res, x, y, x+w, y+w))

    x = x0
    y = y0 + yspacing * 2
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Cut.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Paste.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Copy.png'), res, x, y, x+w, y+w))

    x = x0
    y = y0 + yspacing * 3
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Play.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'FastForward.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Stop.png'), res, x, y, x+w, y+w))

    x = x0
    y = y0 + yspacing * 4
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Up.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Down.png'), res, x, y, x+w, y+w))

    x = x0
    y = y0 + yspacing * 5
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Pencil.png'), res, x, y, x+w, y+w))
    x += xspacing
    os.system('inkscape --export-png {1} --export-area={3}:{4}:{5}:{6} -w {2} {0}'.format(
        filename, os.path.join(outpath, 'Pen.png'), res, x, y, x+w, y+w))
