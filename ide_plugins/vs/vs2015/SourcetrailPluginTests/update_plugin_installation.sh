#!/bin/bash

VSIX_PATH="D:/sourcetrail/sourcetrail/ide_plugins/vs/vs2015/SourcetrailPlugin/SourcetrailPlugin/bin/vs2015/Release/sourcetrail_plugin_vs2015.vsix"

SOURCETRAIL_PLUGIN_ID="acf15780-03b5-440e-a41e-db79b7043fc2"


echo -e "trying to uninstall old version of plugin"
cmd.exe /c "VsDevCmd.bat & START /WAIT VSIXInstaller.exe /skuName:Pro /skuVersion:14.0 /uninstall:$SOURCETRAIL_PLUGIN_ID"
echo -e "\tuninstall done"

echo -e ""

echo -e "trying to install new version of plugin"
cmd.exe /c "VsDevCmd.bat & START /WAIT VSIXInstaller.exe /quiet /skuName:Pro /skuVersion:14.0 $VSIX_PATH"
echo -e "\tinstall done"


#"devenv.com" /command exit