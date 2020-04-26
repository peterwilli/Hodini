cmake_minimum_required (VERSION 2.8.11)
project (Hodini)

find_package(X11 REQUIRED)
message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
include_directories(${PROJECT_SOURCE_DIR})
FIND_PACKAGE( Boost 1.40 COMPONENTS program_options system REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
add_executable (hodini main.cpp)
TARGET_LINK_LIBRARIES(hodini ${X11_LIBRARIES} ${Boost_LIBRARIES})