
# upx
execute_process(
	COMMAND upx --brute ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati -o ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati_upx
)

execute_process(
	COMMAND upx --brute ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati_trial -o
	${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release/Coati_trial_upx
)

