CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libdynamixel C)

ADD_SUBDIRECTORY(src)

#TODO: add stand alone target for examples

#ADD_CUSTOM_TARGET(examples SOURCES examples/CMakeLists.txt)
IF (WITH_EXAMPLES)
	ADD_SUBDIRECTORY(examples)
ENDIF (WITH_EXAMPLES)

IF (WITH_PYTHONSWIG)
	ADD_SUBDIRECTORY(contrib/python-swig)
ENDIF (WITH_PYTHONSWIG)

INSTALL (FILES src/dynamixel.h DESTINATION include)
INSTALL (FILES src/libdynamixel.a DESTINATION lib)

IF (WITH_SHARED)
	INSTALL (FILES src/libdynamixel_shared.so DESTINATION lib)
ENDIF (WITH_SHARED)


