#!/bin/sh

# get the current directory
SOURCE="${0}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
	DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
	SOURCE="$(readlink "$SOURCE")"
	if [ $SOURCE != /* ]; then
		SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
	fi
done
SOURCETRAIL_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

export LD_LIBRARY_PATH="$SOURCETRAIL_PATH/lib:$LD_LIBRARY_PATH"
export QT_XKB_CONFIG_ROOT="/usr/share/X11/xkb:$QT_XKB_CONFIG_ROOT"
export QT_QPA_PLATFORM_PLUGIN_PATH="$SOURCETRAIL_PATH/lib/platforms"
export QT_QPA_FONTDIR="$SOURCETRAIL_PATH/data/fonts:$QT_QPA_FONTDIR"
export SOURCETRAIL_VIA_SCRIPT=1
exec "$SOURCETRAIL_PATH/Sourcetrail" $@
