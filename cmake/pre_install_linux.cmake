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

# upx
	set(upxPath ${CMAKE_CURRENT_LIST_DIR}/../build/Release/app)

	execute_process(
		COMMAND ${upxPath}/Coati -z ${CMAKE_CURRENT_LIST_DIR}/../setup/license.txt
	)

	execute_process(
		COMMAND ${upxPath}/Coati -f -d ${CMAKE_CURRENT_LIST_DIR}/../bin/app/user/projects/tictactoe/tictactoe.coatiproject
	)

	execute_process(
		COMMAND ${upxPath}/Coati -f -d ${CMAKE_CURRENT_LIST_DIR}/../bin/app/user/projects/tutorial/tutorial.coatiproject
	)

	execute_process(
		COMMAND ${upxPath}/Coati -f -d ${CMAKE_CURRENT_LIST_DIR}/../bin/app/user/projects/javaparser/javaparser.coatiproject
	)

