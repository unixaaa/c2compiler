CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(c2compiler)

SET(CMAKE_CXX_COMPILER "clang++")

#SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_VERBOSE_MAKEFILE true)
ADD_DEFINITIONS(-Wall)
#ADD_DEFINITIONS(-Wall -Wextra -Wshadow -Wno-overloaded-virtual -Wno-unused-parameter)

SET(CMAKE_VERBOSE_MAKEFILE false)

ADD_SUBDIRECTORY(c2c)
ADD_SUBDIRECTORY(tools)

ADD_CUSTOM_TARGET(tests
    COMMAND ${CMAKE_BINARY_DIR}/tools/tester test
    DEPENDS c2c tester
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

