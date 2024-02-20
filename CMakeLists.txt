cmake_minimum_required(VERSION 3.15)
set(project_name EuterpeSnd)
project(${project_name})


include(CMakePrintHelpers)
include(cmake/Sanitiziers.cmake)
include(cmake/CmakeHelpers.cmake)
include(cmake/TestingHelpers.cmake)
include(cmake/StaticAnalyzers.cmake)

option(BUILD_APPS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set_property(GLOBAL PROPERTY USE_FOLDER ON)
enable_testing()
add_subdirectory(deps/googletest)
add_subdirectory(deps/rtaudio)
add_subdirectory(utilities)
add_subdirectory(dsp)
add_subdirectory(synthtools)

if(BUILD_APPS)
    add_subdirectory(audioDevice)
    add_subdirectory(apps)
endif()


