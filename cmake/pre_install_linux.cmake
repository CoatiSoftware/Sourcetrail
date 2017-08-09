#pre install script
function(Print text)
	execute_process(
		COMMAND echo ${text}
	)
endfunction()

function(Run)
	execute_process(
		COMMAND ${ARGV}
	)
endfunction()

set(binPath ${CMAKE_CURRENT_LIST_DIR}/../build/Release/app/Sourcetrail)
set(projPath ${CMAKE_CURRENT_LIST_DIR}/../bin/app/user/projects)

execute_process(
	COMMAND ${binPath} config -z ${CMAKE_CURRENT_LIST_DIR}/../setup/license.txt
)

execute_process(
	COMMAND ${binPath} index --full ${projPath}/tictactoe/tictactoe.srctrlprj
)

execute_process(
	COMMAND ${binPath} index --full ${projPath}/tutorial/tutorial.srctrlprj
)

execute_process(
	COMMAND ${binPath} index --full ${projPath}/javaparser/javaparser.srctrlprj
)

