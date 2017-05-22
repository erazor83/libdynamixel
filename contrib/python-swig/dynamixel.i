%module dynamixel
%include "typemaps.i"
%include "carrays.i"
%include "cstring.i"

%typemap(in) uint32_t {
	$1 = PyInt_AsLong($input);
}
%typemap(in) uint8_t,uint16_t {
	$1 = PyInt_AsLong($input)&0xff;
}
%typemap(in) uint16_t {
	$1 = PyInt_AsLong($input)&0xffff;
}
%typemap(out) int8_t {
	$result = PyInt_FromLong($1);
}
	
%typemap(in) (uint8_t length, uint8_t* data){
	if (PyList_Check($input)) {
		int i=0;
		$1=PyList_Size($input);
		$2 = (uint8_t*) malloc(($1+1)*sizeof(uint8_t));
		//printf("creating uint8_t with size %i -> %i\n",$1,($1*sizeof(uint8_t)));
		for (i=0; i<$1;i++) {
			PyObject *o = PyList_GetItem($input,i);
			if (PyInt_Check(o)) {
				//printf("1\n");
				$2[i]=(uint8_t)PyInt_AsLong(PyList_GetItem($input,i));
			} else {
				PyErr_SetString(PyExc_TypeError,"list must contain integers");
				free($2);
				return NULL;
			}
			//printf("2\n");
		}
		//printf("3\n");
	} else {
		PyErr_SetString(PyExc_TypeError,"not a list");
		return NULL;
	}
}

%typemap(in,numinputs=1) (uint8_t length,uint8_t** dst) {
	$1 = PyInt_AsLong($input)&0xff;
	$2 = (uint8_t**)malloc(sizeof(uint8_t*));
}

%typemap(argout) (uint8_t length, uint8_t** dst) {
	int w;
	
	PyObject *values;

	values=PyList_New($1);

	if (*$2) {
		for (w=0;w<$1;w++) {
			PyList_SetItem(values,w,PyInt_FromLong((long) (*$2)[w]));
		}
	}
	free($2);
	%append_output(values);
}

%typemap(argout) (dynamixel_register_t address, uint8_t* dst) {
	%append_output(PyInt_FromLong((long) $2[0]));
}
%typemap(argout) (dynamixel_register_t address, uint16_t* dst) {
	%append_output(PyInt_FromLong((long) $2[0]));
}

%{
#define SWIG_FILE_WITH_INIT
#include "dynamixel.h"
#include "dynamixel-rtu.h"
%}

%feature("autodoc","1");


%include "dynamixel.h"
%include "dynamixel-rtu.h"
%inline %{
	// helper to create a new dynamixel context
	struct dynamixel_t* dynamixel_ctx(void) {
		return (struct dynamixel_t *) malloc(sizeof(struct dynamixel_t*));
	}
%}