# FLAGS
CLEAN_AND_SETUP=true
REBUILD=true
RUN_CODE_SIGNING=false
UPDATE_DATABASES=true
CREATE_WIX_INSTALLER=true
CREATE_PORTABLE_PACKAGE=true


# USEFUL VARIABLES
ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

ORIGINAL_PATH_TO_SCRIPT="${0}"
CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
BASE_DIR=`dirname "$CLEANED_PATH_TO_SCRIPT"`
OUTPUT_FOLDER="release"


cd $BASE_DIR/..

if [ $CLEAN_AND_SETUP = false ]; then
	echo -e "$INFO CLEAN_AND_SETUP flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $REBUILD = false ]; then
	echo -e "$INFO REBUILD flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $RUN_CODE_SIGNING = false ]; then
	echo -e "$INFO RUN_CODE_SIGNING flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $UPDATE_DATABASES = false ]; then
	echo -e "$INFO UPDATE_DATABASES flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $CREATE_WIX_INSTALLER = false ]; then
	echo -e "$INFO CREATE_WIX_INSTALLER flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $CREATE_PORTABLE_PACKAGE = false ]; then
	echo -e "$INFO CREATE_PORTABLE_PACKAGE flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi


VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"
DOTTED_VERSION_STRING="${VERSION_STRING//_/.}"


if [ $RUN_CODE_SIGNING = true ]; then
	echo -e "$INFO Code signing is enabled. provide the key's sh1 thumbprint >"
	read CODE_SIGNING_THUMBPRINT
fi


if [ $CLEAN_AND_SETUP = true ]; then
	echo -e "$INFO Cleaning the project"
	script/clean.sh
	
	echo -e $INFO "$INFO Running cmake with 64 bit configuration"
	mkdir -p build
	cd build
	cmake -G "Visual Studio 15 2017 Win64" -DBOOST_ROOT=${BOOST_ROOT} -DQt5_DIR=${Qt5_64_DIR} -DClang_DIR=${Clang_64_DIR} -DBUILD_CXX_LANGUAGE_PACKAGE=ON -DBUILD_JAVA_LANGUAGE_PACKAGE=ON -DBUILD_PYTHON_LANGUAGE_PACKAGE=ON ..
	cd ..
fi


# CLEANING THE SOLUTIONS
if [ $REBUILD = true ]; then
	echo -e "$INFO Cleaning the solution"
	"devenv.com" build/Sourcetrail.sln //clean Release
fi


# BUILDING THE EXECUTABLES
echo -e "$INFO Building the project"
"devenv.com" build/Sourcetrail.sln //build Release //project build/Sourcetrail.vcxproj

if [ $RUN_CODE_SIGNING = true ]; then
	echo -e "$INFO Signing the app"
	signtool.exe sign //sha1 $CODE_SIGNING_THUMBPRINT //fd sha256 //t "http://timestamp.comodoca.com/authenticode" //d "Sourcetrail ${DOTTED_VERSION_STRING}" //du "https://www.sourcetrail.com/" //v build/Release/app/Sourcetrail.exe
	
	echo -e "$INFO Signing the indexer"
	signtool.exe sign //sha1 $CODE_SIGNING_THUMBPRINT //fd sha256 //t "http://timestamp.comodoca.com/authenticode" //d "Sourcetrail Indexer ${DOTTED_VERSION_STRING}" //du "https://www.sourcetrail.com/" //v build/Release/app/sourcetrail_indexer.exe
	
	echo -e "$INFO Signing the Python indexer"
	signtool.exe sign //sha1 $CODE_SIGNING_THUMBPRINT //fd sha256 //t "http://timestamp.comodoca.com/authenticode" //d "Sourcetrail Python Indexer" //du "https://github.com/CoatiSoftware/SourcetrailPythonIndexer" //v bin/app/data/python/SourcetrailPythonIndexer.exe
fi


rm -rf ./$OUTPUT_FOLDER
mkdir ./$OUTPUT_FOLDER


# STORING PDB FILES
echo -e "$INFO storing pdb file"
mkdir ./$OUTPUT_FOLDER/PDB
cp -u -r ./build/Release/app/Sourcetrail.pdb ./$OUTPUT_FOLDER/PDB/


# CREATING DATABASES
if [ $UPDATE_DATABASES = true ]; then
	echo -e "$INFO creating databases"
	
	rm bin/app/user/projects/tutorial/tutorial.srctrldb
	rm bin/app/user/projects/tictactoe_cpp/tictactoe_cpp.srctrldb
	rm bin/app/user/projects/tictactoe_py/tictactoe_py.srctrldb
	rm bin/app/user/projects/javaparser/javaparser.srctrldb
	rm -rf temp

	mkdir -p temp
	cd temp

	echo -e "$INFO configuring application"
	../build/Release/app/Sourcetrail.exe config -t 8

	echo -e "$INFO creating database for tutorial"
	../build/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/tutorial/tutorial.srctrlprj

	echo -e "$INFO creating database for tictactoe_cpp"
	../build/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/tictactoe_cpp/tictactoe_cpp.srctrlprj
	
	echo -e "$INFO creating database for tictactoe_py"
	../build/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/tictactoe_py/tictactoe_py.srctrlprj

	echo -e "$INFO creating database for javaparser"
	../build/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/javaparser/javaparser.srctrlprj

	cd ..
	rm -rf temp
fi


# BUILDING THE INSTALLER
cd deployment/windows/wixSetup
sh build.sh -win64
[ $? -eq 0 ]  || exit 1
cd ../../..


if [ $RUN_CODE_SIGNING = true ]; then
	echo -e "$INFO Signing the 64 bit windows installer"
	signtool.exe sign //sha1 $CODE_SIGNING_THUMBPRINT //fd sha256 //t "http://timestamp.comodoca.com/authenticode" //d "Sourcetrail ${DOTTED_VERSION_STRING} Installer" //du "https://www.sourcetrail.com/" //v deployment/windows/wixSetup/bin/sourcetrail.msi
fi


# CREATING THE INSTALLER ZIP FILES
if [ $CREATE_WIX_INSTALLER = true ]; then
	# CREATING INSTALLER PACKAGE FOLDER
	echo -e "$INFO creating installer package folder"
	INSTALLER_PACKAGE_NAME=Sourcetrail_${VERSION_STRING}_64bit_Installer
	INSTALLER_PACKAGE_DIR=$OUTPUT_FOLDER/$INSTALLER_PACKAGE_NAME
	INSTALLER_PACKAGE_PLUGINS_DIR=$INSTALLER_PACKAGE_DIR/plugins

	rm -rf $INSTALLER_PACKAGE_DIR
	mkdir -p $INSTALLER_PACKAGE_DIR

	cp -u -r deployment/windows/wixSetup/bin/* $INSTALLER_PACKAGE_DIR/

	# COPYING PLUGINS DATA
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/atom/
	cp -u -r ide_plugins/atom/* $INSTALLER_PACKAGE_PLUGINS_DIR/atom/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/eclipse/
	cp -u -r ide_plugins/eclipse/* $INSTALLER_PACKAGE_PLUGINS_DIR/eclipse/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/emacs/
	cp -u -r ide_plugins/emacs/* $INSTALLER_PACKAGE_PLUGINS_DIR/emacs/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/idea/
	cp -u -r ide_plugins/idea/* $INSTALLER_PACKAGE_PLUGINS_DIR/idea/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/qt_creator/
	cp -u -r ide_plugins/qt_creator/* $INSTALLER_PACKAGE_PLUGINS_DIR/qt_creator/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/sublime_text/
	cp -u -r ide_plugins/sublime_text/* $INSTALLER_PACKAGE_PLUGINS_DIR/sublime_text/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/vim/
	cp -u -r ide_plugins/vim/* $INSTALLER_PACKAGE_PLUGINS_DIR/vim/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/vscode/
	cp -u -r ide_plugins/vscode/* $INSTALLER_PACKAGE_PLUGINS_DIR/vscode/
	mkdir -p $INSTALLER_PACKAGE_PLUGINS_DIR/visual_studio/
	cp -u -r ide_plugins/visual_studio/* $INSTALLER_PACKAGE_PLUGINS_DIR/visual_studio/

	cd ./$OUTPUT_FOLDER/

	# PACKAGING SOURCETRAIL
	echo -e "$INFO packaging sourcetrail installer"
	winrar a -afzip Sourcetrail_${VERSION_STRING}_Windows_64bit_Installer.zip $INSTALLER_PACKAGE_NAME

	cd ../

	# CLEANING UP
	echo -e "$INFO Cleaning installer package folders"
	rm -rf $INSTALLER_PACKAGE_DIR	
fi


# CREATING THE PORTABLE ZIP FILES
if [ $CREATE_PORTABLE_PACKAGE = true ]; then
	# CREATING PORTABLE PACKAGE FOLDER
	echo -e "$INFO creating portable package folder"
	PORTABLE_PACKAGE_NAME=Sourcetrail_${VERSION_STRING}_64bit_Portable
	PORTABLE_PACKAGE_DIR=$OUTPUT_FOLDER/$PORTABLE_PACKAGE_NAME
	PORTABLE_PACKAGE_APP_DIR=$PORTABLE_PACKAGE_DIR/Sourcetrail_${VERSION_STRING}_64bit
	PORTABLE_PACKAGE_PLUGINS_DIR=$PORTABLE_PACKAGE_DIR/plugins

	rm -rf $PORTABLE_PACKAGE_DIR
	mkdir -p $PORTABLE_PACKAGE_DIR


	# COPYING APPLICATION DATA
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/license/3rd_party_licenses/
	cp -u -r bin/app/data/license/3rd_party_licenses/* $PORTABLE_PACKAGE_APP_DIR/data/license/3rd_party_licenses/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/color_schemes/
	cp -u -r bin/app/data/color_schemes/* $PORTABLE_PACKAGE_APP_DIR/data/color_schemes/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/cxx/
	cp -u -r bin/app/data/cxx/* $PORTABLE_PACKAGE_APP_DIR/data/cxx/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/fallback/
	cp -u -r bin/app/data/fallback/* $PORTABLE_PACKAGE_APP_DIR/data/fallback/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/fonts/
	cp -u -r bin/app/data/fonts/* $PORTABLE_PACKAGE_APP_DIR/data/fonts/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/gui/
	cp -u -r bin/app/data/gui/* $PORTABLE_PACKAGE_APP_DIR/data/gui/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/install/
	cp -u -r bin/app/data/install/* $PORTABLE_PACKAGE_APP_DIR/data/install/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/java/
	cp -u -r bin/app/data/java/* $PORTABLE_PACKAGE_APP_DIR/data/java/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/python/
	cp -u -r bin/app/data/python/* $PORTABLE_PACKAGE_APP_DIR/data/python/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/syntax_highlighting_rules/
	cp -u -r bin/app/data/syntax_highlighting_rules/* $PORTABLE_PACKAGE_APP_DIR/data/syntax_highlighting_rules/

	mkdir -p $PORTABLE_PACKAGE_APP_DIR/user/projects/javaparser/
	cp -u -r bin/app/user/projects/javaparser/* $PORTABLE_PACKAGE_APP_DIR/user/projects/javaparser/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/user/projects/tictactoe_cpp/
	cp -u -r bin/app/user/projects/tictactoe_cpp/* $PORTABLE_PACKAGE_APP_DIR/user/projects/tictactoe_cpp/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/user/projects/tictactoe_py/
	cp -u -r bin/app/user/projects/tictactoe_py/* $PORTABLE_PACKAGE_APP_DIR/user/projects/tictactoe_py/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/user/projects/tutorial/
	cp -u -r bin/app/user/projects/tutorial/* $PORTABLE_PACKAGE_APP_DIR/user/projects/tutorial/

	mkdir -p $PORTABLE_PACKAGE_APP_DIR/imageformats/
	cp -u -r build/Release/app/imageformats/*.dll $PORTABLE_PACKAGE_APP_DIR/imageformats/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/platforms/
	cp -u -r build/Release/app/platforms/*.dll $PORTABLE_PACKAGE_APP_DIR/platforms/
	mkdir -p $PORTABLE_PACKAGE_APP_DIR/styles/
	cp -u -r build/Release/app/styles/*.dll $PORTABLE_PACKAGE_APP_DIR/styles/

	cp -u -r build/Release/app/*.dll $PORTABLE_PACKAGE_APP_DIR/
	cp -u -r build/Release/app/qt.conf $PORTABLE_PACKAGE_APP_DIR/
	cp -u -r build/Release/app/Sourcetrail.exe $PORTABLE_PACKAGE_APP_DIR/
	cp -u -r build/Release/app/sourcetrail_indexer.exe $PORTABLE_PACKAGE_APP_DIR/


	# COPYING PLUGINS DATA
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/atom/
	cp -u -r ide_plugins/atom/* $PORTABLE_PACKAGE_PLUGINS_DIR/atom/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/eclipse/
	cp -u -r ide_plugins/eclipse/* $PORTABLE_PACKAGE_PLUGINS_DIR/eclipse/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/emacs/
	cp -u -r ide_plugins/emacs/* $PORTABLE_PACKAGE_PLUGINS_DIR/emacs/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/idea/
	cp -u -r ide_plugins/idea/* $PORTABLE_PACKAGE_PLUGINS_DIR/idea/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/qt_creator/
	cp -u -r ide_plugins/qt_creator/* $PORTABLE_PACKAGE_PLUGINS_DIR/qt_creator/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/sublime_text/
	cp -u -r ide_plugins/sublime_text/* $PORTABLE_PACKAGE_PLUGINS_DIR/sublime_text/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/vim/
	cp -u -r ide_plugins/vim/* $PORTABLE_PACKAGE_PLUGINS_DIR/vim/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/vscode/
	cp -u -r ide_plugins/vscode/* $PORTABLE_PACKAGE_PLUGINS_DIR/vscode/
	mkdir -p $PORTABLE_PACKAGE_PLUGINS_DIR/visual_studio/
	cp -u -r ide_plugins/visual_studio/* $PORTABLE_PACKAGE_PLUGINS_DIR/visual_studio/

	cd ./$OUTPUT_FOLDER/

	# PACKAGING SOURCETRAIL
	echo -e "$INFO packaging sourcetrail portable"
	winrar a -afzip Sourcetrail_${VERSION_STRING}_Windows_64bit_Portable.zip $PORTABLE_PACKAGE_NAME

	cd ../

	# CLEANING UP
	echo -e "$INFO Cleaning portable package folders"
	rm -rf $PORTABLE_PACKAGE_DIR
fi


echo -e "$SUCCESS packaging complete!"
