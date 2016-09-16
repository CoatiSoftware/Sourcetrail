# FLAGS
REBUILD=true
OBFUSCATE=true


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

echo -e "$INFO Did you set the correct version number for the installers?"
read -p "Press [Enter] key to continue"

echo -e "$INFO Did you change the Product-IDs in the uninstall scripts?"
read -p "Press [Enter] key to continue"

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
	"D:/programme/Microsoft Visual Studio14/Common7/IDE/devenv.com" build/Coati.sln //clean Release
fi


# BUILDING THE EXECUTABLES
echo -e "$INFO building the executable (app)"
"D:/programme/Microsoft Visual Studio14/Common7/IDE/devenv.com" build/Coati.sln //build Release //project build/Coati.vcxproj

echo -e "$INFO building the executable (trail)"
"D:/programme/Microsoft Visual Studio14/Common7/IDE/devenv.com" build/Coati.sln //build Release //project build/Coati_trial.vcxproj


# CREATING TRAIL DATABASES´
echo -e "$INFO creating databases for trail"

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


# OBFUSCATING THE EXECUTABLES
if [ $OBFUSCATE = true ]; then
	echo -e "$INFO obfuscating the executables"
	rm bin/app/Release/Coati_obfuscated.exe
	rm bin/app/Release/Coati_trial_obfuscated.exe
	upx --brute -o bin/app/Release/Coati_obfuscated.exe bin/app/Release/Coati.exe
	upx --brute -o bin/app/Release/Coati_trial_obfuscated.exe bin/app/Release/Coati_trial.exe
else
	cp bin/app/Release/Coati.exe bin/app/Release/Coati_obfuscated.exe
	cp bin/app/Release/Coati_trial.exe bin/app/Release/Coati_trial_obfuscated.exe
fi


# BUILDING THE INSTALLERS
echo -e "$INFO building the installer (app)"
"D:/programme/Microsoft Visual Studio14/Common7/IDE/devenv.com" deployment/windows/CoatiAppSetup/CoatiAppSetup.sln //build Release //project deployment/windows/CoatiAppSetup/CoatiSetup/CoatiSetup.vdproj

echo -e "$INFO building the installer (trail)"
"D:/programme/Microsoft Visual Studio14/Common7/IDE/devenv.com" deployment/windows/CoatiTrialSetup/CoatiTrialSetup.sln //build Release //project deployment/windows/CoatiTrialSetup/CoatiSetup/CoatiSetup.vdproj


# BUILDING WIX INSTALLERS
# call deployment/windows/wixSetup/build.bat


# EDIT THE INSTALLERS
"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Bin/MsiTran.Exe" -a "deployment/windows/transform.mst" "deployment/windows/CoatiAppSetup/CoatiSetup/Release/Coati.msi"
"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Bin/MsiTran.Exe" -a "deployment/windows/transform.mst" "deployment/windows/CoatiTrialSetup/CoatiSetup/Release/CoatiTrial.msi"


# CREATING PACKAGE FOLDERS (APP)
VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"

APP_PACKAGE_NAME=Coati_${VERSION_STRING}

APP_PACKAGE_DIR=release/$APP_PACKAGE_NAME

echo -e "$INFO creating package folders for app"
rm -rf $APP_PACKAGE_DIR
mkdir -p $APP_PACKAGE_DIR

cp -u -r deployment/windows/CoatiAppSetup/CoatiSetup/Release/Coati.msi $APP_PACKAGE_DIR/
cp -u -r deployment/windows/CoatiAppSetup/CoatiSetup/Release/setup.exe $APP_PACKAGE_DIR/

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


# PACKAGING COATI (APP)
echo -e "$INFO packaging coati app"
cd ./release/
winrar a -afzip ${APP_PACKAGE_NAME}_Windows.zip $APP_PACKAGE_NAME
cd ../


# CLEANING UP (APP)
echo -e "$INFO Cleaning up app"
rm -rf $APP_PACKAGE_DIR


# CREATING PACKAGE FOLDERS (TRIAL)
TRIAL_PACKAGE_NAME=Coati_Trial_${VERSION_STRING}

TRIAL_PACKAGE_DIR=release/$TRIAL_PACKAGE_NAME

echo -e "$INFO creating package folders for trail"
rm -rf $TRIAL_PACKAGE_DIR
mkdir -p $TRIAL_PACKAGE_DIR

cp -u -r deployment/windows/CoatiTrialSetup/CoatiSetup/Release/CoatiTrial.msi $TRIAL_PACKAGE_DIR/
cp -u -r deployment/windows/CoatiTrialSetup/CoatiSetup/Release/setup.exe $TRIAL_PACKAGE_DIR/

mkdir -p $TRIAL_PACKAGE_DIR/plugins/atom/
cp -u -r ide_plugins/atom/* $TRIAL_PACKAGE_DIR/plugins/atom/
mkdir -p $TRIAL_PACKAGE_DIR/plugins/eclipse/
cp -u -r ide_plugins/eclipse/* $TRIAL_PACKAGE_DIR/plugins/eclipse/
mkdir -p $TRIAL_PACKAGE_DIR/plugins/emacs/
cp -u -r ide_plugins/emacs/* $TRIAL_PACKAGE_DIR/plugins/emacs/
mkdir -p $TRIAL_PACKAGE_DIR/plugins/idea/
cp -u -r ide_plugins/idea/* $TRIAL_PACKAGE_DIR/plugins/idea/
mkdir -p $TRIAL_PACKAGE_DIR/plugins/sublime_text/
cp -u -r ide_plugins/sublime_text/* $TRIAL_PACKAGE_DIR/plugins/sublime_text/
mkdir -p $TRIAL_PACKAGE_DIR/plugins/vim/
cp -u -r ide_plugins/vim/* $TRIAL_PACKAGE_DIR/plugins/vim/
mkdir -p $TRIAL_PACKAGE_DIR/plugins/visual_studio/
cp -u -r ide_plugins/vs/coati_plugin_vs.vsix $TRIAL_PACKAGE_DIR/plugins/visual_studio/


# PACKAGING COATI (TRIAL)
echo -e "$INFO packaging coati trail"
cd ./release/
winrar a -afzip ${TRIAL_PACKAGE_NAME}_Windows.zip $TRIAL_PACKAGE_NAME
cd ../


# CLEANING UP (TRIAL)
echo -e "$INFO Cleaning up trail"
rm -rf $TRIAL_PACKAGE_DIR


# UNSETTING THE DEPLOY FLAG
rm -rf src/lib_gui/platform_includes/deploy.h
echo "// #define DEPLOY" >src/lib_gui/platform_includes/deploy.h


echo -e "$SUCCESS packaging complete!"




















