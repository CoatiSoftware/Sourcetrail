VERSION_STRING=$(git describe --long)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"
VERSION_STRING="${VERSION_STRING//_/.}"

echo "installer version is $VERSION_STRING"

rm -rf build
mkdir build

"devenv.com" Setup/build/Setup.sln //build "Release|x86"
"devenv.com" CustomActions/CustomActions.sln //build "Release|x86"

rm -rf bin
mkdir bin
mkdir bin\bin

candle.exe -dprojectVersion="$VERSION_STRING" coati.wxs customActions.wxs dialogShortcuts.wxs installDir.wxs appDataDir.wxs -out build/ > build/compileLog.txt
light.exe -ext WixUIExtension build/coati.wixobj build/customActions.wixobj build/dialogShortcuts.wixobj build/installDir.wixobj build/appDataDir.wixobj -out build/coati.msi > build/linkLog.txt

cp -u -r build/coati.msi bin/bin
cp -u -r build/Setup.exe bin
cp -u -r lib/api-ms-win-crt-heap-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-locale-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-math-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-runtime-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-stdio-l1-1-0.dll bin
cp -u -r lib/msvcp140.dll bin
cp -u -r lib/vcruntime140.dll bin
cp -u -r readme.txt bin