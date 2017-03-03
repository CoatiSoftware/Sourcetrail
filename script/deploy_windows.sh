# FLAGS
CLEAN_AND_SETUP=true
REBUILD=true
UPDATE_DATABASES=true


# USEFUL VARIABLES
ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

ORIGINAL_PATH_TO_SCRIPT="${0}"
CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
BASE_DIR=`dirname "$CLEANED_PATH_TO_SCRIPT"`


cd $BASE_DIR/..

if [ $CLEAN_AND_SETUP = false ]; then
	echo -e "$INFO CLEAN_AND_SETUP flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $REBUILD = false ]; then
	echo -e "$INFO REBUILD flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $UPDATE_DATABASES = false ]; then
	echo -e "$INFO UPDATE_DATABASES flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

echo -e "$INFO Did you set the correct version number for the Visual Studio plugin?"
read -p "Press [Enter] key to continue"

echo -e "$INFO Is the Visual Studio plugin up to date?"
read -p "Press [Enter] key to continue"


# SETTING THE DEPLOY FLAG
rm -rf src/lib_gui/platform_includes/deploy.h
echo "#define DEPLOY" >src/lib_gui/platform_includes/deploy.h


### TODO: add HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\MSBuild\EnableOutOfProcBuild


if [ $CLEAN_AND_SETUP = true ]; then
	echo -e "$INFO cleaning the project"
	script/clean.sh
	echo -e "$INFO setting the project up"
	script/setup.sh
fi


# CLEANING THE SOLUTIONS
if [ $REBUILD = true ]; then
	echo -e "$INFO cleaning the solution"
	"devenv.com" build/win32/Coati.sln //clean Release
	"devenv.com" build/win64/Coati.sln //clean Release
fi


# BUILDING THE EXECUTABLES
echo -e "$INFO building the app (32 bit)"
"devenv.com" build/win32/Coati.sln //build Release //project build/win32/Coati.vcxproj
echo -e "$INFO building the app (64 bit)"
"devenv.com" build/win64/Coati.sln //build Release //project build/win64/Coati.vcxproj


# CREATING DATABASES
if [ $UPDATE_DATABASES = true ]; then
	echo -e "$INFO creating databases"

	rm bin/app/user/projects/tictactoe/tictactoe.coatidb
	rm bin/app/user/projects/tutorial/tutorial.coatidb
	rm bin/app/user/projects/javaparser/javaparser.coatidb
	rm -rf temp

	mkdir -p temp
	cd temp

	echo -e "$INFO saving license key"
	../build/win32/Release/app/Coati.exe -z ../script/license.txt

	echo -e "$INFO creating database for tictactoe"
	../build/win32/Release/app/Coati.exe -p ../bin/app/user/projects/tictactoe/tictactoe.coatiproject -d

	echo -e "$INFO creating database for tutorial"
	../build/win32/Release/app/Coati.exe -p ../bin/app/user/projects/tutorial/tutorial.coatiproject -d

	echo -e "$INFO creating database for javaparser"
	../build/win32/Release/app/Coati.exe -p ../bin/app/user/projects/javaparser/javaparser.coatiproject -d

	cd ..
	rm -rf temp
fi


# BUILDING THE INSTALLER
cd deployment/windows/wixSetup
build_win32.sh
build_win64.sh
cd ../../..


# CREATING PACKAGE FOLDER
VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"

echo -e "$INFO creating package folder for win32"
APP_PACKAGE_NAME_WIN32=Coati_${VERSION_STRING}_32bit
APP_PACKAGE_DIR_WIN32=release/$APP_PACKAGE_NAME_WIN32

rm -rf $APP_PACKAGE_DIR_WIN32
mkdir -p $APP_PACKAGE_DIR_WIN32

cp -u -r deployment/windows/wixSetup/bin/win32/* $APP_PACKAGE_DIR_WIN32/

mkdir -p $APP_PACKAGE_DIR_WIN32/plugins/atom/
cp -u -r ide_plugins/atom/* $APP_PACKAGE_DIR_WIN32/plugins/atom/
mkdir -p $APP_PACKAGE_DIR_WIN32/plugins/eclipse/
cp -u -r ide_plugins/eclipse/* $APP_PACKAGE_DIR_WIN32/plugins/eclipse/
mkdir -p $APP_PACKAGE_DIR_WIN32/plugins/emacs/
cp -u -r ide_plugins/emacs/* $APP_PACKAGE_DIR_WIN32/plugins/emacs/
mkdir -p $APP_PACKAGE_DIR_WIN32/plugins/idea/
cp -u -r ide_plugins/idea/* $APP_PACKAGE_DIR_WIN32/plugins/idea/
mkdir -p $APP_PACKAGE_DIR_WIN32/plugins/sublime_text/
cp -u -r ide_plugins/sublime_text/* $APP_PACKAGE_DIR_WIN32/plugins/sublime_text/
mkdir -p $APP_PACKAGE_DIR_WIN32/plugins/vim/
cp -u -r ide_plugins/vim/* $APP_PACKAGE_DIR_WIN32/plugins/vim/
mkdir -p $APP_PACKAGE_DIR_WIN32/plugins/visual_studio/
cp -u -r ide_plugins/vs/coati_plugin_vs.vsix $APP_PACKAGE_DIR_WIN32/plugins/visual_studio/


echo -e "$INFO creating package folder for win64"
APP_PACKAGE_NAME_WIN64=Coati_${VERSION_STRING}_64bit
APP_PACKAGE_DIR_WIN64=release/$APP_PACKAGE_NAME_WIN64

rm -rf $APP_PACKAGE_DIR_WIN64
mkdir -p $APP_PACKAGE_DIR_WIN64

cp -u -r deployment/windows/wixSetup/bin/win64/* $APP_PACKAGE_DIR_WIN64/

mkdir -p $APP_PACKAGE_DIR_WIN64/plugins/atom/
cp -u -r ide_plugins/atom/* $APP_PACKAGE_DIR_WIN64/plugins/atom/
mkdir -p $APP_PACKAGE_DIR_WIN64/plugins/eclipse/
cp -u -r ide_plugins/eclipse/* $APP_PACKAGE_DIR_WIN64/plugins/eclipse/
mkdir -p $APP_PACKAGE_DIR_WIN64/plugins/emacs/
cp -u -r ide_plugins/emacs/* $APP_PACKAGE_DIR_WIN64/plugins/emacs/
mkdir -p $APP_PACKAGE_DIR_WIN64/plugins/idea/
cp -u -r ide_plugins/idea/* $APP_PACKAGE_DIR_WIN64/plugins/idea/
mkdir -p $APP_PACKAGE_DIR_WIN64/plugins/sublime_text/
cp -u -r ide_plugins/sublime_text/* $APP_PACKAGE_DIR_WIN64/plugins/sublime_text/
mkdir -p $APP_PACKAGE_DIR_WIN64/plugins/vim/
cp -u -r ide_plugins/vim/* $APP_PACKAGE_DIR_WIN64/plugins/vim/
mkdir -p $APP_PACKAGE_DIR_WIN64/plugins/visual_studio/
cp -u -r ide_plugins/vs/coati_plugin_vs.vsix $APP_PACKAGE_DIR_WIN64/plugins/visual_studio/


# PACKAGING COATI
echo -e "$INFO packaging coati"
cd ./release/

winrar a -afzip Coati_${VERSION_STRING}_Windows_32bit.zip $APP_PACKAGE_NAME_WIN32
winrar a -afzip Coati_${VERSION_STRING}_Windows_64bit.zip $APP_PACKAGE_NAME_WIN64


# STORING PDB FILES
mkdir PDB_${APP_PACKAGE_NAME_WIN32}
cp -u -r ../build/win32/Release/app/Coati.pdb PDB_${APP_PACKAGE_NAME_WIN32}/
mkdir PDB_${APP_PACKAGE_NAME_WIN64}
cp -u -r ../build/win64/Release/app/Coati.pdb PDB_${APP_PACKAGE_NAME_WIN64}/

cd ../


# CLEANING UP
echo -e "$INFO Cleaning package folders"
rm -rf $APP_PACKAGE_DIR_WIN32
rm -rf $APP_PACKAGE_DIR_WIN64


# UNSETTING THE DEPLOY FLAG
rm -rf src/lib_gui/platform_includes/deploy.h
echo $'// #define DEPLOY' >src/lib_gui/platform_includes/deploy.h


echo -e "$SUCCESS packaging complete!"




















