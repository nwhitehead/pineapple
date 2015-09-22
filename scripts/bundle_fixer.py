'''
Fixup exectable files and dynamic libraries inside bundles so they
are relocatable.

Args:
    python bundle_fixer.py THING WHERE EXECPATH

THING to fixup (file)
WHERE to put dylibs (directory)
EXECPATH is where we assume @executable_path points to at runtime

'''

import sys
import os
import string
import subprocess
import shutil

def get_linked(file):
    '''Return array of dynamically linked libraries using otool'''
    output = subprocess.check_output(['/usr/bin/otool', '-L', file])
    lines = output.split('\n')
    lines = lines[1:-1] # first line is report, last line is blank
    lines = [line.lstrip() for line in lines]
    def strip_compat(txt):
        paren = string.find(txt, '(')
        return txt[0:paren-1]
    lines = [strip_compat(line) for line in lines]
    return lines

def is_lib(lib):
    '''Determine if a file is a dynamic library (heuristic)'''
    if lib.endswith('.so'): return True
    if lib.endswith('.dylib'): return True
    return False

def should_copy_lib(lib):
    '''Determine if a library should be copied (heuristic)'''
    if lib.startswith('@'): return False
    if lib.startswith('/System'): return False
    if lib.startswith('/usr/lib'): return False
    if lib.startswith('/usr/lib'): return False
    return True

def pathsplit(path):
    '''Split string path into array of parts'''
    return os.path.normpath(path).split(os.sep)

def compute_relative_array(base, ext):
    '''Compute relative path to get from ext to base (everything is arrays)'''
    prefix = []
    ext = ext[:-1] # assume ext is an actual file
    while len(base) > 0 and len(ext) > 0 and base[0] == ext[0]:
        prefix.append(base[0])
        base = base[1:]
        ext = ext[1:]
    res = []
    while len(ext) > 0:
        res.append('..')
        ext = ext[1:]
    while len(base) > 0:
        res.append(base[0])
        base = base[1:]
    return res

def compute_relative(base, ext):
    '''Compute relative path between string paths'''
    return os.sep.join(compute_relative_array(pathsplit(base), pathsplit(ext)))

def install_name_tool(thing, oldlib, newlib):
    '''Use install_name_tool to update dynamic linking info in thing'''
    return subprocess.check_call(['/usr/bin/install_name_tool', '-change', oldlib, newlib, thing])

def relocate(thing, libloc):
    print("### Relocating {}".format(thing))
    libs = get_linked(thing)
    for lib in libs:
        if should_copy_lib(lib):
            libname = os.path.split(lib)[1]
            print('libname = ' + libname)
            print("### ### Copying {}".format(lib))
            if not os.path.exists(libloc):
                os.makedirs(libloc)
            destname = os.path.join(libloc, libname)
            if os.path.isfile(destname):
                print('### ### (destination already exists, skipping)')
            else:
                shutil.copy(lib, libloc)
            install_name_tool(thing, lib, '@loader_path/' + compute_relative(destname, thing))

def relocate_dir(root, libloc):
    '''Relocate all libraries inside root dir'''
    for subdir, dirs, files in os.walk(root):
        for file in files:
            fname = os.path.join(subdir, file)
            if is_lib(fname):
                #print('### NOT IMPLEMENTED file = {}'.format(fname))
                relocate(fname, libloc)
        # Recurse into subdirectories
        for dir in dirs:
            relocate_dir(os.path.join(root, dir), libloc)

if __name__ == '__main__':
    print("Welcome to the bundle fixer")
    print("---------------------------")
    thing = sys.argv[1]
    print("### Thing to fixup: {}".format(thing))
    libloc = sys.argv[2]
    print("### Where to put dylibs: {}".format(libloc))
    if os.path.isfile(thing):
        relocate(thing, libloc)
    else:
        print("### DIRECTORY")
        relocate_dir(thing, libloc)
