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

    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND ${var_name} "${CMAKE_SOURCE_DIR}/${_relPath}/${_src}")
        else()
            list (APPEND ${var_name} "${CMAKE_SOURCE_DIR}/${_src}")
        endif()
    endforeach()

    if (_relPath)
        set(${var_name} ${${var_name}} PARENT_SCOPE)
    else()
        set(${var_name} ${${var_name}})
    endif()
endmacro()
