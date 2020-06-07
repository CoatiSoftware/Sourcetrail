#!/bin/sh

BIN_PATH=build/Release/app/Sourcetrail
PROJECTS_PATH=bin/app/user/projects

./$BIN_PATH index --full $PROJECTS_PATH/tictactoe_cpp/tictactoe_cpp.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/tictactoe_py/tictactoe_py.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/tutorial/tutorial.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/javaparser/javaparser.srctrlprj
