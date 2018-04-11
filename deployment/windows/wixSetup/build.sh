WIN_ARCH=win32
X_ARCH=x86

if [ "$1" == "-win64" ]; then
	WIN_ARCH=win64
	X_ARCH=x64
fi

VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"
VERSION_STRING="${VERSION_STRING//_/.}"
VERSION_STRING="${VERSION_STRING:2}"
echo "installer version is $VERSION_STRING"

PRODUCT_GUID=$(cmd.exe /c "VsDevCmd.bat & uuidgen")
echo "$WIN_ARCH product guid is $PRODUCT_GUID"

rm -rf build
mkdir build


rm -rf ../../../bin/app/data/install/uninstall_wix_$WIN_ARCH.bat
echo $"%windir%\system32\msiexec.exe /x {$PRODUCT_GUID}" >../../../bin/app/data/install/uninstall_wix_$WIN_ARCH.bat


"devenv.com" CustomActions/CustomActions.sln //build "Release|$X_ARCH"

OUTPUT_DIR=bin/$WIN_ARCH

rm -rf $OUTPUT_DIR
mkdir -p $OUTPUT_DIR

candle.exe -dProductVersion="$VERSION_STRING" -dProductGuid="$PRODUCT_GUID" -arch $X_ARCH sourcetrail.wxs customActions.wxs dialogShortcuts.wxs installDir.wxs appDataDir.wxs -out build/ > build/compileLog.txt
light.exe -ext WixUIExtension build/sourcetrail.wixobj build/customActions.wixobj build/dialogShortcuts.wixobj build/installDir.wixobj build/appDataDir.wixobj -out build/sourcetrail.msi > build/linkLog.txt

cp -u -r build/sourcetrail.msi $OUTPUT_DIR
cp -u -r readme.txt $OUTPUT_DIR