#!/bin/sh

if [ "$(id -u)" -ne 0 ]; then
  echo >&2 "Error: Please run script as root or with sudo. Exiting..."
  exit 1
fi

echo "Installing Sourcetrail in /opt/sourcetrail"

MY_PATH="$( cd "$( dirname "$0" )" && pwd )"

rm -rf /opt/sourcetrail > /dev/null

cp -rf "$MY_PATH/" /opt/sourcetrail > /dev/null
cp -rf "$MY_PATH/share/applications" /usr/share > /dev/null
cp -rf "$MY_PATH/share/icons" /usr/share > /dev/null
cp -rf "$MY_PATH/share/mime" /usr/share > /dev/null

cp "$MY_PATH/share/data/gui/icon/logo_1024_1024.png" /usr/share/icons/sourcetrail.png > /dev/null
cp "$MY_PATH/share/data/gui/icon/project_256_256.png" /usr/share/icons/project-sourcetrail.png > /dev/null

ln -f -s /opt/sourcetrail/Sourcetrail.sh /usr/bin/sourcetrail

update-mime-database /usr/share/mime > /dev/null
update-desktop-database > /dev/null

mkdir -p ~/.config/sourcetrail
if [ "$SUDO_USER" ]; then CHOWNUSER=$SUDO_USER; else CHOWNUSER=$USER; fi
chown -R $CHOWNUSER ~/.config/sourcetrail

echo "Installation complete."
echo "Enter 'sourcetrail' to launch application."
