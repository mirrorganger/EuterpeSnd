cmake_minimum_required(VERSION 3.15)
set(project_name EuterpeSnd)
project(${project_name})


include(CMakePrintHelpers)
include(cmake/Sanitiziers.cmake)
include(cmake/CmakeHelpers.cmake)
include(cmake/TestingHelpers.cmake)
include(cmake/StaticAnalyzers.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set_property(GLOBAL PROPERTY USE_FOLDER ON)
enable_testing()
add_subdirectory(deps/googletest)
add_subdirectory(utilities)
add_subdirectory(dsp)
add_subdirectory(synthtools)





