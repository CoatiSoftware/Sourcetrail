macro(create_source_groups)
    foreach(FILE ${ARGN}) 
		get_filename_component(PARENT_DIR "${FILE}" PATH)
		# skip src or include and changes /'s to \\'s
		string(REGEX REPLACE "(\\./)?(src|include)/?" "" GROUP "${PARENT_DIR}")
		string(REPLACE "/" "\\" GROUP "${GROUP}")
		
		string(LENGTH "${GROUP}" GROUP_NAME_LENGTH)
		if (${GROUP_NAME_LENGTH} GREATER 0)
			source_group("${GROUP}" FILES "${FILE}")
		endif()
	endforeach()
endmacro()
