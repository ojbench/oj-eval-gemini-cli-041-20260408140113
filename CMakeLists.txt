cmake_minimum_required(VERSION 3.10)
project(map_extra)

file(GLOB SOURCES "*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dummy.cpp")

if(SOURCES)
    add_executable(code ${SOURCES})
else()
    add_executable(code dummy.cpp)
endif()
