#!/bin/sh

if [ "$(id -u)" -ne 0 ]; then
  echo >&2 "Error: Please run script as root or with sudo. Exiting..."
  exit 1
fi

echo "Do you want to uninstall Sourcetrail?"

printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

rm /usr/bin/sourcetrail
rm /usr/share/mime/packages/sourcetrail-mime.xml
rm /usr/share/applications/sourcetrail.desktop
rm /usr/share/icons/sourcetrail.png
rm /usr/share/icons/project-sourcetrail.png
rm /opt/sourcetrail -rf
update-mime-database /usr/share/mime
update-desktop-database

echo "Deinstallation complete."
