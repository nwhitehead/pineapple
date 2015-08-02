#!/usr/bin/env python

##################################
# Kernel imports
##################################

import numpy

##################################
import sys

from notebook.notebookapp import main
from ipykernel import kernelapp

if __name__ == '__main__':
    print('Hello from server.py')
    print('I was called with arguments {}'.format(sys.argv))
    if len(sys.argv) >= 2 and sys.argv[1] == '-m' and sys.argv[2] == 'ipykernel':
        print('I should make myself a Python kernel')
        print('Launching')
        kernelapp.launch_new_instance()
    else:
        main()
