devenv.exe /build "Release|x86" Setup/build/Setup.sln
devenv.exe /build "Release|x86" CustomActions/CustomActions.sln

rm -rf bin
mkdir bin

candle.exe -dprojectVersion="0.7.0" coati.wxs customActions.wxs dialogShortcuts.wxs installDir.wxs appDataDir.wxs > compileLog.txt
light.exe -ext WixUIExtension coati.wixobj customActions.wixobj dialogShortcuts.wixobj installDir.wixobj appDataDir.wixobj -out coati.msi > linkLog.txt

cp -u -r coati.msi bin
cp -u -r Setup.exe bin
