#!/bin/sh

# get the current directory
SOURCE="${0}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
	DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
	SOURCE="$(readlink "$SOURCE")"
	[[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
COATI_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

#setup data folder
if [ ! -d "${HOME}/.config/coati" ]; then
	# Control will enter here if $DIRECTORY doesn't exist.
	mkdir -p "${HOME}/.config/coati"
	echo "First start of Coati from this user, copy default configfiles to ~/.config/coati"
fi

cp -rn "$COATI_PATH/user/*" "${HOME}/.config/coati/"

export LD_LIBRARY_PATH="$COATI_PATH/lib:LD_LIBRARY_PATH"
export QT_XKB_CONFIG_ROOT="/usr/share/X11/xkb:$QT_XKB_CONFIG_ROOT"
export QT_QPA_FONTDIR="$COATI_PATH/data/fonts:$QT_QPA_FONTDIR"
exec "$COATI_PATH/Coati" $@
