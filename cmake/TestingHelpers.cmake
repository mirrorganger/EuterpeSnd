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

	set(unit_test_stamp ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}_stamp)
	add_custom_command(OUTPUT ${unit_test_stamp}
		DEPENDS ${EXECUTABLE_TEST}
		POST_BUILD
		COMMAND ${EXECUTABLE_TEST} ${GTEST_COMMON_ARGS}
		COMMAND ${CMAKE_COMMAND} -E touch ${unit_test_stamp}
	)

    add_test(NAME ${EXECUTABLE_TEST} COMMAND ${EXECUTABLE_TEST})

    add_custom_target(${EXECUTABLE_TEST}_run ALL DEPENDS ${unit_test_stamp} ${EXECUTABLE_TEST})
    add_dependencies(COMPILE_TEST ${EXECUTABLE_TEST}_run)

    
    include_directories({${LIBRARY_PATH}}src)
    target_link_libraries(${EXECUTABLE_TEST} ${LIBRARY_NAME})
ENDFUNCTION()
