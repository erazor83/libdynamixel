%module dynamixel
%include "typemaps.i"
%include "carrays.i"
%include "cstring.i"

/*
	TODO: add typeconverters for pointers
	see svn://ed-solutions.de/e-design/pub/pylibmodbus how I did this for libmodbus
*/
%{
#include "dynamixel.h"
%}
%include "dynamixel.h"
