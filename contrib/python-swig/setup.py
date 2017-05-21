#!/usr/bin/env python

"""
setup.py file for libdynamixel bindings.
"""

from distutils.core import setup,Extension

dynamixel_module = Extension('_dynamixel',
	libraries=['dynamixel_shared'],
	sources=['dynamixel_wrap.c'],
	)

setup (name = 'dynamixel',
	version = '0.1.1',
	author = "Alexander Krause",
	description = """Simple libdynamixel bindings.""",
	ext_modules = [dynamixel_module],
	py_modules = ["dynamixel"],
)
