ORIGINAL_PATH_TO_SCRIPT="${0}"
CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
SCRIPT_DIR=`dirname "$CLEANED_PATH_TO_SCRIPT"`
ROOT_DIR=$SCRIPT_DIR/../../..

WIN_ARCH=win32
X_ARCH=x86
WIN_64=no

if [ "$1" == "-win64" ]; then
	WIN_ARCH=win64
	X_ARCH=x64
	WIN_64=yes
fi

VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"
VERSION_STRING="${VERSION_STRING//_/.}"
VERSION_STRING="${VERSION_STRING:2}"
echo "installer version is $VERSION_STRING"

PRODUCT_GUID=$(cmd.exe /c "uuidgen")
if [ -z "$PRODUCT_GUID" ]; then
	echo "Unable to generate GUID. Make sure to run this script from the Visual Studio command propmt. Aborting now."
	exit 1
else
	echo "$WIN_ARCH product guid is $PRODUCT_GUID"
fi

rm -rf build
mkdir build


rm -rf $ROOT_DIR/bin/app/data/install/uninstall_wix_$WIN_ARCH.bat
echo $"%windir%\system32\msiexec.exe /x {$PRODUCT_GUID}" >$ROOT_DIR/bin/app/data/install/uninstall_wix_$WIN_ARCH.bat


"devenv.com" CustomActions/CustomActions.sln //build "Release|$X_ARCH"

OUTPUT_DIR=bin/$WIN_ARCH

rm -rf $OUTPUT_DIR
mkdir -p $OUTPUT_DIR

# user
SAMPLE_JAVAPARSER_DIR=$ROOT_DIR/bin/app/user/projects/javaparser
SAMPLE_TICTACTOE_CPP_DIR=$ROOT_DIR/bin/app/user/projects/tictactoe_cpp
SAMPLE_TICTACTOE_PY_DIR=$ROOT_DIR/bin/app/user/projects/tictactoe_py
SAMPLE_TUTORIAL_DIR=$ROOT_DIR/bin/app/user/projects/tutorial

heat.exe dir $SAMPLE_JAVAPARSER_DIR -cg SampleJavaparserComponentGroup -var var.SampleJavaparserSourceDir -out build/sampleJavaparser.wxs -gg -sfrag -g1 -dr SampleProjects -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $SAMPLE_TICTACTOE_CPP_DIR -cg SampleTictactoeCppComponentGroup -var var.SampleTictactoeCppSourceDir -out build/sampleTictactoeCpp.wxs -gg -sfrag -g1 -dr SampleProjects -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $SAMPLE_TICTACTOE_PY_DIR -cg SampleTictactoePyComponentGroup -var var.SampleTictactoePySourceDir -out build/sampleTictactoePy.wxs -gg -sfrag -g1 -dr SampleProjects -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $SAMPLE_TUTORIAL_DIR -cg SampleTutorialComponentGroup -var var.SampleTutorialSourceDir -out build/sampleTutorial.wxs -gg -sfrag -g1 -dr SampleProjects -t $SCRIPT_DIR/HeatTransform.xslt


# data
DATA_COLOR_SCHEMES_DIR=$ROOT_DIR/bin/app/data/color_schemes
DATA_CXX_DIR=$ROOT_DIR/bin/app/data/cxx
DATA_FALLBACK_DIR=$ROOT_DIR/bin/app/data/fallback
DATA_FONTS_DIR=$ROOT_DIR/bin/app/data/fonts
DATA_GUI_DIR=$ROOT_DIR/bin/app/data/gui
DATA_JAVA_DIR=$ROOT_DIR/bin/app/data/java
DATA_PYTHON_DIR=$ROOT_DIR/bin/app/data/python
DATA_SYNTAX_HIGHLIGHTING_RULES_DIR=$ROOT_DIR/bin/app/data/syntax_highlighting_rules


heat.exe dir $DATA_COLOR_SCHEMES_DIR -cg DataColorSchemesComponentGroup -var var.DataColorSchemesSourceDir -out build/dataColorSchemes.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $DATA_CXX_DIR -cg DataCxxComponentGroup -var var.DataCxxSourceDir -out build/dataCxx.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $DATA_FALLBACK_DIR -cg DataFallbackComponentGroup -var var.DataFallbackSourceDir -out build/dataFallback.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $DATA_FONTS_DIR -cg DataFontsComponentGroup -var var.DataFontsSourceDir -out build/dataFonts.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $DATA_GUI_DIR -cg DataGuiComponentGroup -var var.DataGuiSourceDir -out build/dataGui.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $DATA_JAVA_DIR -cg DataJavaComponentGroup -var var.DataJavaSourceDir -out build/dataJava.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $DATA_PYTHON_DIR -cg DataPythonComponentGroup -var var.DataPythonSourceDir -out build/dataPython.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt
heat.exe dir $DATA_SYNTAX_HIGHLIGHTING_RULES_DIR -cg DataSyntaxHighlightingRulesComponentGroup -var var.DataSyntaxHighlightingRulesSourceDir -out build/dataSyntaxHighlightingRules.wxs -gg -sfrag -g1 -dr LieutenantCommanderData -t $SCRIPT_DIR/HeatTransform.xslt


candle.exe -dProductVersion="$VERSION_STRING" -dProductGuid="$PRODUCT_GUID" -dWin64="$WIN_64" -arch $X_ARCH -out build/ \
	-dSampleJavaparserSourceDir="$SAMPLE_JAVAPARSER_DIR" build/sampleJavaparser.wxs \
	-dSampleTictactoeCppSourceDir="$SAMPLE_TICTACTOE_CPP_DIR" build/sampleTictactoeCpp.wxs \
	-dSampleTictactoePySourceDir="$SAMPLE_TICTACTOE_PY_DIR" build/sampleTictactoePy.wxs \
	-dSampleTutorialSourceDir="$SAMPLE_TUTORIAL_DIR" build/sampleTutorial.wxs \
	-dDataColorSchemesSourceDir="$DATA_COLOR_SCHEMES_DIR" build/dataColorSchemes.wxs \
	-dDataCxxSourceDir="$DATA_CXX_DIR" build/dataCxx.wxs \
	-dDataFallbackSourceDir="$DATA_FALLBACK_DIR" build/dataFallback.wxs \
	-dDataFontsSourceDir="$DATA_FONTS_DIR" build/dataFonts.wxs \
	-dDataGuiSourceDir="$DATA_GUI_DIR" build/dataGui.wxs \
	-dDataJavaSourceDir="$DATA_JAVA_DIR" build/dataJava.wxs \
	-dDataPythonSourceDir="$DATA_PYTHON_DIR" build/dataPython.wxs \
	-dDataSyntaxHighlightingRulesSourceDir="$DATA_SYNTAX_HIGHLIGHTING_RULES_DIR" build/dataSyntaxHighlightingRules.wxs \
	sourcetrail.wxs customActions.wxs dialogShortcuts.wxs installDir.wxs appDataDir.wxs \
	> build/compileLog.txt
	
	
light.exe -ext WixUIExtension \
	build/sampleJavaparser.wixobj \
	build/sampleTictactoeCpp.wixobj \
	build/sampleTictactoePy.wixobj \
	build/sampleTutorial.wixobj \
	build/dataColorSchemes.wixobj \
	build/dataCxx.wixobj \
	build/dataFallback.wixobj \
	build/dataFonts.wixobj \
	build/dataGui.wixobj \
	build/dataJava.wixobj \
	build/dataPython.wixobj \
	build/dataSyntaxHighlightingRules.wixobj \
	build/sourcetrail.wixobj build/customActions.wixobj build/dialogShortcuts.wixobj build/installDir.wixobj build/appDataDir.wixobj -out build/sourcetrail.msi \
	> build/linkLog.txt

cp -u -r build/sourcetrail.msi $OUTPUT_DIR
cp -u -r readme.txt $OUTPUT_DIR