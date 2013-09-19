#!/bin/sh
CFLAGS="-I. -I../../src -L../../src" python setup.py build_ext --inplace
 
swig -python dynamixel.i
python setup.py build_ext --inplace
