#
# Xin YUAN, 2019, BSD (2)
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
INCLUDE("../csl-base.cmk")

PROJECT(CSL-LIBS)

ADD_DEFINITIONS(${STRING_CXX_FLAGS})

# CSL

ADD_SUBDIRECTORY(src src)

ADD_SUBDIRECTORY(auxiliary auxiliary)

ADD_SUBDIRECTORY(tools tools)

# end
