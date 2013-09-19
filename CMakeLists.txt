CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libdynamixel C)

ADD_SUBDIRECTORY(src)

#TODO: add stand alone target for examples

#ADD_CUSTOM_TARGET(examples SOURCES examples/CMakeLists.txt)
ADD_SUBDIRECTORY(examples)

ADD_SUBDIRECTORY(contrib/python-swig)
