add_custom_target(ALL_COMPILE)

FUNCTION(find_files_matching_patterns output directory filter_masks)
	set( file_list )	
	foreach( filter_mask ${filter_masks} )
		file(GLOB_RECURSE found_files ${directory}/${filter_mask})
		list(APPEND file_list ${found_files})
		endforeach()
	set(${output} ${file_list} PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(__add_dependencies TARGET TARGET_DEPENDS TARGET_DEPENDS_NO_TARGET)
	foreach( dependency ${TARGET_DEPENDS} ) 
		if (NOT TARGET ${dependency})
			message(FATAL_ERROR "The dependency target \"${dependency}\" of target \"${TARGET}\" does not exist, please check and reorder the add_subdirectory() base on dependency")
		endif() 
		target_link_libraries(${TARGET} PUBLIC ${dependency})
	endforeach()
	foreach( dependency ${TARGET_DEPENDS_NO_TARGET} ) 
		target_link_libraries(${TARGET} PUBLIC ${dependency})
	endforeach()
ENDFUNCTION()

FUNCTION(setup_library)
	set(options UNIT_TEST)
	set(oneValueArgs NAME PATH COMPONENT)
	set(multiValueArgs DEPENDS EXTERNAL_DEPENDS)
	
	cmake_parse_arguments(LIBRARY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if (NOT "${LIBRARY_PATH}" STREQUAL "")
		set(LIBRARY_PATH "${LIBRARY_PATH}/")
	endif()

	set(HEADER_FILES_FILTER_MASK *.h *.hpp)
	set(SOURCE_FILES_FILTER_MASK *.cpp *.cc)

	find_files_matching_patterns(publicIncludes ${LIBRARY_PATH}include "${HEADER_FILES_FILTER_MASK}")
	find_files_matching_patterns(privateIncludes ${LIBRARY_PATH}src "${HEADER_FILES_FILTER_MASK}")
	find_files_matching_patterns(src ${LIBRARY_PATH}src "${SOURCE_FILES_FILTER_MASK}")
	
	add_library(${LIBRARY_NAME} ${publicIncludes} ${src} ${privateIncludes})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_PATH}include")
		message(STATUS "${LIBRARY_PATH}")
		target_include_directories(${LIBRARY_NAME} PUBLIC ${LIBRARY_PATH}include)
	endif()
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_PATH}src")
		target_include_directories(${LIBRARY_NAME} PRIVATE ${LIBRARY_PATH}src)
	endif()

	__add_dependencies(${LIBRARY_NAME} "${LIBRARY_DEPENDS}" "${LIBRARY_EXTERNAL_DEPENDS}")
	add_dependencies(ALL_COMPILE ${LIBRARY_NAME})

	#target_compile_options(${LIBRARY_NAME} PRIVATE -Wall -Wextra)
	enable_coverage(${LIBRARY_NAME})

	if(${LIBRARY_UNIT_TEST})
		addTest()
	endif()

ENDFUNCTION()

FUNCTION(setup_executable)
	set(options)
	set(oneValueArgs NAME)
	set(multiValueArgs DEPENDS EXTERNAL_DEPENDS)
	cmake_parse_arguments(EXECUTABLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	set(SOURCE_FILES_FILTER_MASK *.cpp *.cc *.h *.hpp)

	find_files_matching_patterns(EXECUTABLE_SOURCES src "${SOURCE_FILES_FILTER_MASK}")

	add_executable(${EXECUTABLE_NAME} "${EXECUTABLE_SOURCES}")
	set_property(TARGET ${EXECUTABLE_NAME} PROPERTY FOLDER ${EXECUTABLE_NAME})
	
	add_dependencies(ALL_COMPILE ${EXECUTABLE_NAME})

	__add_dependencies(${EXECUTABLE_NAME} "${EXECUTABLE_DEPENDS}" "${EXECUTABLE_EXTERNAL_DEPENDS}")

	target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra)

	install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_BINARY_DIR}/products)
ENDFUNCTION()
