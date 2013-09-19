#!/usr/bin/env python

"""
setup.py file for libdynamixel bindings.
"""

from distutils.core import setup,Extension

libdynamixel_module = Extension('_libdynamixel',
	libraries=['dynamixel_shared'],
	sources=['dynamixel_wrap.c'],
	)

setup (name = 'libdynamixel',
	version = '0.1.0',
	author = "Alexander Krause",
	description = """Simple libdynamixel bindings.""",
	ext_modules = [libdynamixel_module],
	py_modules = ["libdynamixel"],
)
