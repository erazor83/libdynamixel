#!/bin/sh

swig -I../../src  -python dynamixel.i
CFLAGS="-I../../src -L../../src" python setup.py build_ext --inplace
