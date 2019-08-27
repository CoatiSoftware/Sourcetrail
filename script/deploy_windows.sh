# FLAGS
CLEAN_AND_SETUP=true
REBUILD=true
RUN_CODE_SIGNING=true
UPDATE_DATABASES=true
CREATE_INSTALLER_ZIP=true
CREATE_PORTABLE_ZIP=true


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

if [ $RUN_CODE_SIGNING = false ]; then
	echo -e "$INFO RUN_CODE_SIGNING flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $UPDATE_DATABASES = false ]; then
	echo -e "$INFO UPDATE_DATABASES flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $CREATE_INSTALLER_ZIP = false ]; then
	echo -e "$INFO CREATE_INSTALLER_ZIP flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi

if [ $CREATE_PORTABLE_ZIP = false ]; then
	echo -e "$INFO CREATE_PORTABLE_ZIP flag is set to false. Do you want to proceed?"
	read -p "Press [Enter] key to continue"
fi


### TODO: add HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\MSBuild\EnableOutOfProcBuild


VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"
DOTTED_VERSION_STRING="${VERSION_STRING//_/.}"


if [ $RUN_CODE_SIGNING = true ]; then
	echo -e "$INFO Code signing is enabled. Please enter your password >"
	read CODE_SIGNING_PASSWORD
fi


if [ $CLEAN_AND_SETUP = true ]; then
	echo -e "$INFO Cleaning the project"
	script/clean.sh
	echo -e "$INFO Setting up the project"
	script/setup.sh
	
	if [ $RUN_CODE_SIGNING = true ]; then
		echo -e "$INFO Signing the Python indexer"
		signtool.exe sign //f "deployment/windows/certificate/coati_software_kg.pfx" //p $CODE_SIGNING_PASSWORD //t "http://timestamp.verisign.com/scripts/timstamp.dll" //d "Sourcetrail Python Indexer" //du "https://github.com/CoatiSoftware/SourcetrailPythonIndexer" //v bin/app/data/python/SourcetrailPythonIndexer.exe
	fi
fi


# CLEANING THE SOLUTIONS
if [ $REBUILD = true ]; then
	echo -e "$INFO Cleaning the solution"
	"devenv.com" build/win32/Sourcetrail.sln //clean Release
	"devenv.com" build/win64/Sourcetrail.sln //clean Release
fi


# BUILDING THE EXECUTABLES
build_executable() # Parameters: bit {32, 64}
{
	echo -e "$INFO Building the app (${1} bit)"
	"devenv.com" build/win${1}/Sourcetrail.sln //build Release //project build/win${1}/Sourcetrail.vcxproj
	
	if [ $RUN_CODE_SIGNING = true ]; then
		echo -e "$INFO Signing the app (${1} bit)"
		signtool.exe sign //f "deployment/windows/certificate/coati_software_kg.pfx" //p $CODE_SIGNING_PASSWORD //t "http://timestamp.verisign.com/scripts/timstamp.dll" //d "Sourcetrail ${DOTTED_VERSION_STRING}" //du "https://www.sourcetrail.com/" //v build/win${1}/Release/app/Sourcetrail.exe
		
		echo -e "$INFO Signing the indexer (${1} bit)"
		signtool.exe sign //f "deployment/windows/certificate/coati_software_kg.pfx" //p $CODE_SIGNING_PASSWORD //t "http://timestamp.verisign.com/scripts/timstamp.dll" //d "Sourcetrail Indexer ${DOTTED_VERSION_STRING}" //du "https://www.sourcetrail.com/" //v build/win${1}/Release/app/sourcetrail_indexer.exe
	fi
}
build_executable "32"
build_executable "64"


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
	../build/win32/Release/app/Sourcetrail.exe accept-eula
	../build/win32/Release/app/Sourcetrail.exe config -t 8
	../build/win32/Release/app/Sourcetrail.exe config -M 1024

	echo -e "$INFO creating database for tutorial"
	../build/win32/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/tutorial/tutorial.srctrlprj

	echo -e "$INFO creating database for tictactoe_cpp"
	../build/win32/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/tictactoe_cpp/tictactoe_cpp.srctrlprj
	
	echo -e "$INFO creating database for tictactoe_py"
	../build/win32/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/tictactoe_py/tictactoe_py.srctrlprj

	echo -e "$INFO creating database for javaparser"
	../build/win32/Release/app/Sourcetrail.exe index --full --project-file ../bin/app/user/projects/javaparser/javaparser.srctrlprj

	cd ..
	rm -rf temp
fi


# BUILDING THE INSTALLER
cd deployment/windows/wixSetup
sh build_win32.sh
sh build_win64.sh
cd ../../..


if [ $RUN_CODE_SIGNING = true ]; then
	echo -e "$INFO Signing the 32 bit windows installer"
	signtool.exe sign //f "deployment/windows/certificate/coati_software_kg.pfx" //p $CODE_SIGNING_PASSWORD //t "http://timestamp.verisign.com/scripts/timstamp.dll" //d "Sourcetrail ${DOTTED_VERSION_STRING} Installer" //du "https://www.sourcetrail.com/" //v deployment/windows/wixSetup/bin/win32/sourcetrail.msi
	
	echo -e "$INFO Signing the 64 bit windows installer"
	signtool.exe sign //f "deployment/windows/certificate/coati_software_kg.pfx" //p $CODE_SIGNING_PASSWORD //t "http://timestamp.verisign.com/scripts/timstamp.dll" //d "Sourcetrail ${DOTTED_VERSION_STRING} Installer" //du "https://www.sourcetrail.com/" //v deployment/windows/wixSetup/bin/win64/sourcetrail.msi
fi


# CREATING THE INSTALLER ZIP FILES
if [ $CREATE_INSTALLER_ZIP = true ]; then
	create_installer_zip() # Parameters: bit {32, 64}
	{
		# CREATING INSTALLER PACKAGE FOLDER
		echo -e "$INFO creating installer package folder for win$1"
		local INSTALLER_PACKAGE_NAME=Sourcetrail_${VERSION_STRING}_${1}bit_Installer
		local INSTALLER_PACKAGE_DIR=release/$INSTALLER_PACKAGE_NAME
		local INSTALLER_PACKAGE_PLUGINS_DIR=$INSTALLER_PACKAGE_DIR/plugins

		rm -rf $INSTALLER_PACKAGE_DIR
		mkdir -p $INSTALLER_PACKAGE_DIR

		cp -u -r deployment/windows/wixSetup/bin/win${1}/* $INSTALLER_PACKAGE_DIR/

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

		cd ./release/

		# PACKAGING SOURCETRAIL
		echo -e "$INFO packaging sourcetrail installer for win$1"
		winrar a -afzip Sourcetrail_${VERSION_STRING}_Windows_${1}bit_Installer.zip $INSTALLER_PACKAGE_NAME

		# STORING PDB FILES
		echo -e "$INFO storing pdb file for win$1"
		mkdir PDB_${INSTALLER_PACKAGE_NAME}
		cp -u -r ../build/win$1/Release/app/Sourcetrail.pdb PDB_${INSTALLER_PACKAGE_NAME}/

		cd ../

		# CLEANING UP
		echo -e "$INFO Cleaning installer package folders for win$1"
		rm -rf $INSTALLER_PACKAGE_DIR
	}

	create_installer_zip "32"
	create_installer_zip "64"
fi


# CREATING THE PORTABLE ZIP FILES
if [ $CREATE_PORTABLE_ZIP = true ]; then
	create_portable_zip() # Parameters: bit {32, 64}
	{
		# CREATING PORTABLE PACKAGE FOLDER
		echo -e "$INFO creating portable package folder for win$1"
		local PORTABLE_PACKAGE_NAME=Sourcetrail_${VERSION_STRING}_${1}bit_Portable
		local PORTABLE_PACKAGE_DIR=release/$PORTABLE_PACKAGE_NAME
		local PORTABLE_PACKAGE_APP_DIR=$PORTABLE_PACKAGE_DIR/Sourcetrail_${VERSION_STRING}_${1}bit
		local PORTABLE_PACKAGE_PLUGINS_DIR=$PORTABLE_PACKAGE_DIR/plugins

		rm -rf $PORTABLE_PACKAGE_DIR
		mkdir -p $PORTABLE_PACKAGE_DIR


		# COPYING APPLICATION DATA
		mkdir -p $PORTABLE_PACKAGE_APP_DIR/data/license/3rd_party_licenses/
		cp -u -r bin/app/data/license/3rd_party_licenses/* $PORTABLE_PACKAGE_APP_DIR/data/license/3rd_party_licenses/
		cp -u -r bin/app/data/license/EULA.txt $PORTABLE_PACKAGE_APP_DIR/data/license/
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
		cp -u -r build/win$1/Release/app/imageformats/*.dll $PORTABLE_PACKAGE_APP_DIR/imageformats/
		mkdir -p $PORTABLE_PACKAGE_APP_DIR/platforms/
		cp -u -r build/win$1/Release/app/platforms/*.dll $PORTABLE_PACKAGE_APP_DIR/platforms/

		cp -u -r build/win$1/Release/app/*.dll $PORTABLE_PACKAGE_APP_DIR/
		cp -u -r build/win$1/Release/app/qt.conf $PORTABLE_PACKAGE_APP_DIR/
		cp -u -r build/win$1/Release/app/Sourcetrail.exe $PORTABLE_PACKAGE_APP_DIR/
		cp -u -r build/win$1/Release/app/sourcetrail_indexer.exe $PORTABLE_PACKAGE_APP_DIR/


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

		cd ./release/

		# PACKAGING SOURCETRAIL
		echo -e "$INFO packaging sourcetrail portable for win$1"
		winrar a -afzip Sourcetrail_${VERSION_STRING}_Windows_${1}bit_Portable.zip $PORTABLE_PACKAGE_NAME

		cd ../

		# CLEANING UP
		echo -e "$INFO Cleaning portable package folders for win$1"
		rm -rf $PORTABLE_PACKAGE_DIR
	}

	create_portable_zip "32"
	create_portable_zip "64"
fi


echo -e "$SUCCESS packaging complete!"




















