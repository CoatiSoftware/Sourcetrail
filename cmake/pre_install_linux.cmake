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
	set(upxPath ${CMAKE_CURRENT_LIST_DIR}/../bin/app/Release)

	execute_process(
		COMMAND ${upxPath}/Coati -z $ENV{HOME}/dev/license.txt
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
SET (NO_UPX 1)

if (NO_UPX)
	execute_process(
		COMMAND echo "fdsafsdf"
	)

	execute_process(
		COMMAND rm ${upxPath}/Coati_upx
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${upxPath}/Coati ${upxPath}/Coati_upx
	)
else()
	Print("Remove old Coati_upx")
	Run(rm ${upxPath}/Coati_upx)
	Print("upx Coati")
	Run(upx --brute ${upxPath}/Coati -o ${upxPath}/Coati_upx)
endif()

