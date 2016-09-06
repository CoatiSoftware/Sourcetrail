
# upx
set(upxPath ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release)

execute_process(
	COMMAND ${upxPath}/Coati -z /home/vagrant/dev/license.txt
)

execute_process(
	COMMAND ${upxPath}/Coati -f -d ${upxPath}/../data/projects/tictactoe/tictactoe.coatiproject
)

execute_process(
	COMMAND ${upxPath}/Coati -f -d ${upxPath}/../data/projects/tutorial/tutorial.coatiproject
)

#execute_process(
	#COMMAND ${upxPath}/Coati -f -d ${upxPath}/../data/projects/javaparser/javaparser.coatiproject
#)

set(bla 0)

if (bla)

	message(STATUS "upx the app")
	execute_process(
		COMMAND rm ${upxPath}/Coati_upx
		COMMAND upx --brute ${upxPath}/Coati -o ${upxPath}/Coati_upx
	)

	message(STATUS "upx the trial")
	execute_process(
		COMMAND rm ${upxPath}/Coati_trial_upx
		COMMAND upx --brute ${upxPath}/Coati_trial -o ${upxPath}/Coati_trial_upx
	)

else()
	execute_process(
		COMMAND rm ${upxPath}/Coati_trial_upx
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${upxPath}/Coati_trial ${upxPath}/Coati_trial_upx
		COMMAND rm ${upxPath}/Coati_upx
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${upxPath}/Coati ${upxPath}/Coati_upx
	)

endif()
#add_custom_command(
	#OUTPUT Coati_upx
	#COMMAND upx --brute ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati -o ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati_upx

	#)

