option(ENABLE_CPPCHECK "Enables static analysis with cppcheck tool" OFF)
option(ENABLE_CLANG_TIDY "Enables static analysis with clang-tidy tool" OFF)

if(ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
        message(STATUS "cppcheck tool found")
        set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK}
        --suppress=missingInclude
        --enable=all
        --inline-suppr
        --inconclusive)
    else()
        message(SEND_ERROR "cppcheck enabled but no binary found")
    endif()
endif()

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)
    if(CLANG_TIDY)
        message(STATUS "clang-tidy tool found")
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY} -extra-arg=-Wno-unknown-warning-option)
    else()
        message(SEND_ERROR "clang-tidy enabled but no binary found")
    endif()
endif()