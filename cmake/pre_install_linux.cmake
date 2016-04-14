
# upx
set(upxPath ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release)
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


execute_process(
	COMMAND ${upxPath}/Coati -f -d ${upxPath}/../data/projects/tictactoe/tictactoe.coatiproject
)

execute_process(
	COMMAND ${upxPath}/Coati -f -d ${upxPath}/../data/projects/tutorial/tutorial.coatiproject
)

#add_custom_command(
	#OUTPUT Coati_upx
	#COMMAND upx --brute ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati -o ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati_upx

	#)

