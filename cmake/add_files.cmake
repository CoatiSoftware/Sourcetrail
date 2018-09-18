#
# Defines a list of files with a given name, prepends the relative path to the
# source dir to each file if available and passes the list to PARENT_SCOPE.
#
# Usage:
#	add_files(
#		NAME_OF_FILES
#		file1.cpp
#		file2.cpp
#		...
#		)
#
macro(add_files var_name)
	file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	
# extract source file paths
	set(files_var_name ${var_name}_FILES)
	
	foreach (_src ${ARGN})
		if (_relPath)
			list (APPEND ${files_var_name} "${_relPath}/${_src}")
		else()
			list (APPEND ${files_var_name} "${_src}")
		endif()
	endforeach()
	
# extract source file directories
	set(include_paths_var_name ${var_name}_INCLUDE_PATHS)
	
	foreach (_src ${${files_var_name}})
		get_filename_component(PARENT_DIR "${_src}" DIRECTORY)
		list (APPEND ${include_paths_var_name} "${CMAKE_SOURCE_DIR}/${PARENT_DIR}")
	endforeach()
	
	list(REMOVE_DUPLICATES ${include_paths_var_name})

	if (_relPath)
		set(${files_var_name} ${${files_var_name}} PARENT_SCOPE)
		set(${include_paths_var_name} ${${include_paths_var_name}} PARENT_SCOPE)
	else()
		set(${files_var_name} ${${files_var_name}})
		set(${include_paths_var_name} ${${include_paths_var_name}})
	endif()

endmacro()
