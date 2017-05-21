%module dynamixel
%include "typemaps.i"
%include "carrays.i"
%include "cstring.i"

/*
	TODO: add typeconverters for pointers
	see svn://ed-solutions.de/e-design/pub/pylibmodbus how I did this for libmodbus
*/
%{
#define SWIG_FILE_WITH_INIT
#include "dynamixel.h"
typedef struct dynamixel_t* dynamixel_ctx;
%}

%feature("autodoc","1");


%import "dynamixel.h"
typedef struct dynamixel_t* dynamixel_ctx;
