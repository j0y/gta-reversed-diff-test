cmake_minimum_required(VERSION 3.20)
project(null_d3d9 LANGUAGES CXX)

add_library(d3d9 SHARED null_d3d9.cpp null_d3d9.def)

target_compile_definitions(d3d9 PRIVATE WIN32 _WINDOWS INITGUID)
target_compile_options(d3d9 PRIVATE /EHsc /W3)
target_link_options(d3d9 PRIVATE /SUBSYSTEM:WINDOWS)

set_target_properties(d3d9 PROPERTIES
    OUTPUT_NAME "d3d9"
    SUFFIX ".dll"
)
