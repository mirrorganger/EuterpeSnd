option(ENABLE_COVERAGE "Enables code coverage support analysis from GNU or Clang" OFF)

function(enable_coverage TARGET_NAME)
    if(ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            message(STATUS "Coverage analysis enabled")
            target_compile_options(${TARGET_NAME} INTERFACE --coverage -O0 -g)
            target_link_libraries(${TARGET_NAME} INTERFACE --coverage)
        else()
            message(SEND_ERROR "Code coverage enabled but no support for it found")
        endif()
    else()
        message(STATUS "Coverage analysis disabled")
    endif()
endfunction()