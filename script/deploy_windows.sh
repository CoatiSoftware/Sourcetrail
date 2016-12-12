# FLAGS
REBUILD=true
OBFUSCATE=true
UPDATE_DATABASES=true


# USEFUL VARIABLES
ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

ORIGINAL_PATH_TO_SCRIPT="${0}"
CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
BASE_DIR=`dirname "$CLEANED_PATH_TO_SCRIPT"`


cd $BASE_DIR/..


if [ $REBUILD = false ]; then
	echo -e "$INFO REBUILD flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $OBFUSCATE = false ]; then
	echo -e "$INFO OBFUSCATE flag is set to false. Do you want to proceed?"
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


### TODO: RUN CMAKE!

### TODO: add HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\MSBuild\EnableOutOfProcBuild


# CLEANING THE PROJECT
if [ $REBUILD = true ]; then
	echo -e "$INFO cleaning the project"
	"devenv.com" build/Coati.sln //clean Release
fi


# BUILDING THE EXECUTABLE
echo -e "$INFO building the executable (app)"
"devenv.com" build/Coati.sln //build Release //project build/Coati.vcxproj


# CREATING DATABASES
if [ $UPDATE_DATABASES = true ]; then
	echo -e "$INFO creating databases"

	rm bin/app/data/projects/tictactoe/tictactoe.coatidb
	rm bin/app/data/projects/tutorial/tutorial.coatidb
	rm bin/app/data/projects/javaparser/javaparser.coatidb
	rm -rf temp

	mkdir -p temp
	cd temp

	echo -e "$INFO saving license key"
	../bin/app/Release/Coati.exe -z ../script/license.txt

	echo -e "$INFO creating database for tictactie"
	../bin/app/Release/Coati.exe -p ../bin/app/data/projects/tictactoe/tictactoe.coatiproject -d

	echo -e "$INFO creating database for tutorial"
	../bin/app/Release/Coati.exe -p ../bin/app/data/projects/tutorial/tutorial.coatiproject -d

	echo -e "$INFO creating database for javaparser"
	../bin/app/Release/Coati.exe -p ../bin/app/data/projects/javaparser/javaparser.coatiproject -d

	cd ..
	rm -rf temp
fi


# OBFUSCATING THE EXECUTABLE
if [ $OBFUSCATE = true ]; then
	echo -e "$INFO obfuscating the executables"
	rm bin/app/Release/Coati_obfuscated.exe
	upx --brute -o bin/app/Release/Coati_obfuscated.exe bin/app/Release/Coati.exe
else
	cp bin/app/Release/Coati.exe bin/app/Release/Coati_obfuscated.exe
fi


# BUILDING THE INSTALLER
cd deployment/windows/wixSetup
build.bat
cd ../../..


# CREATING PACKAGE FOLDER
VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"

APP_PACKAGE_NAME=Coati_${VERSION_STRING}

APP_PACKAGE_DIR=release/$APP_PACKAGE_NAME

echo -e "$INFO creating package folder"
rm -rf $APP_PACKAGE_DIR
mkdir -p $APP_PACKAGE_DIR

cp -u -r deployment/windows/wixSetup/bin/* $APP_PACKAGE_DIR/

mkdir -p $APP_PACKAGE_DIR/plugins/atom/
cp -u -r ide_plugins/atom/* $APP_PACKAGE_DIR/plugins/atom/
mkdir -p $APP_PACKAGE_DIR/plugins/eclipse/
cp -u -r ide_plugins/eclipse/* $APP_PACKAGE_DIR/plugins/eclipse/
mkdir -p $APP_PACKAGE_DIR/plugins/emacs/
cp -u -r ide_plugins/emacs/* $APP_PACKAGE_DIR/plugins/emacs/
mkdir -p $APP_PACKAGE_DIR/plugins/idea/
cp -u -r ide_plugins/idea/* $APP_PACKAGE_DIR/plugins/idea/
mkdir -p $APP_PACKAGE_DIR/plugins/sublime_text/
cp -u -r ide_plugins/sublime_text/* $APP_PACKAGE_DIR/plugins/sublime_text/
mkdir -p $APP_PACKAGE_DIR/plugins/vim/
cp -u -r ide_plugins/vim/* $APP_PACKAGE_DIR/plugins/vim/
mkdir -p $APP_PACKAGE_DIR/plugins/visual_studio/
cp -u -r ide_plugins/vs/coati_plugin_vs.vsix $APP_PACKAGE_DIR/plugins/visual_studio/


# PACKAGING COATI
echo -e "$INFO packaging coati"
cd ./release/
winrar a -afzip ${APP_PACKAGE_NAME}_Windows.zip $APP_PACKAGE_NAME
cd ../


# CLEANING UP
echo -e "$INFO Cleaning"
rm -rf $APP_PACKAGE_DIR


# UNSETTING THE DEPLOY FLAG
rm -rf src/lib_gui/platform_includes/deploy.h
echo $'// #define DEPLOY' >src/lib_gui/platform_includes/deploy.h


echo -e "$SUCCESS packaging complete!"




















