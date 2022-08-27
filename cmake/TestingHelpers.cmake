add_custom_target(COMPILE_TEST)

FUNCTION(addTest)
    set(EXECUTABLE_TEST ${LIBRARY_NAME}_test)
    file(GLOB testSrc ${LIBRARY_PATH}test/*c*)
    file(GLOB tesHeaders ${LIBRARY_PATH}test/*h*)

    add_executable(${EXECUTABLE_TEST} ${testSrc} ${testHeaders})
    add_dependencies(ALL_COMPILE ${EXECUTABLE_TEST})

    if(EXISTS ${LIBRARY_PATH}test/data )
        file(COPY ${LIBRARY_PATH}test/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()

    target_link_libraries(${EXECUTABLE_TEST} gmock_main)
    include_directories(${gtest_SOURCE_DIR}/include)

    add_test(NAME ${EXECUTABLE_TEST} COMMAND ${EXECUTABLE_TEST})

    include_directories({${LIBRARY_PATH}}src)
    target_link_libraries(${EXECUTABLE_TEST} ${LIBRARY_NAME})
ENDFUNCTION()
