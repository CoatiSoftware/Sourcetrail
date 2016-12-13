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

candle.exe -dprojectVersion="$VERSION_STRING" coati.wxs customActions.wxs dialogShortcuts.wxs installDir.wxs appDataDir.wxs -out build/ > build/compileLog.txt
light.exe -ext WixUIExtension build/coati.wixobj build/customActions.wixobj build/dialogShortcuts.wixobj build/installDir.wixobj build/appDataDir.wixobj -out build/coati.msi > build/linkLog.txt

cp -u -r build/coati.msi bin
cp -u -r build/Setup.exe bin

cp -u -r lib/api-ms-win-core-console-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-datetime-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-debug-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-errorhandling-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-file-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-file-l1-2-0.dll bin
cp -u -r lib/api-ms-win-core-file-l2-1-0.dll bin
cp -u -r lib/api-ms-win-core-handle-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-heap-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-interlocked-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-libraryloader-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-localization-l1-2-0.dll bin
cp -u -r lib/api-ms-win-core-memory-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-namedpipe-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-processenvironment-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-processthreads-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-processthreads-l1-1-1.dll bin
cp -u -r lib/api-ms-win-core-profile-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-rtlsupport-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-string-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-synch-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-synch-l1-2-0.dll bin
cp -u -r lib/api-ms-win-core-sysinfo-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-timezone-l1-1-0.dll bin
cp -u -r lib/api-ms-win-core-util-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-conio-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-convert-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-environment-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-filesystem-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-heap-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-locale-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-math-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-multibyte-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-private-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-process-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-runtime-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-stdio-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-string-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-time-l1-1-0.dll bin
cp -u -r lib/api-ms-win-crt-utility-l1-1-0.dll bin
cp -u -r lib/msvcp140.dll bin
cp -u -r lib/ucrtbase.dll bin
cp -u -r lib/vcruntime140.dll bin

cp -u -r readme.txt bin