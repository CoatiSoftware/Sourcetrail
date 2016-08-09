devenv.exe /build "Release|x86" Setup/build/Setup.sln
devenv.exe /build "Release|x86" CustomActions/CustomActions.sln

candle.exe coati.wxs customActions.wxs dialogShortcuts.wxs installDir.wxs appDataDir.wxs > compileLog.txt
light.exe -ext WixUIExtension coati.wixobj customActions.wixobj dialogShortcuts.wixobj installDir.wixobj appDataDir.wixobj -out coati.msi > linkLog.txt