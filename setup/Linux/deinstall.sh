
MY_PATH=`dirname "$0"`
MY_PATH=../$MY_PATH

echo "Remove sourcetrail form system"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

echo "Run this script as root"

rm /usr/bin/sourcetrail
rm /usr/share/mime/packages/sourcetrail-mime.xml
rm /usr/share/applications/sourcetrail.desktop
rm /usr/share/icons/sourcetrail.png
rm /usr/share/icons/project-sourcetrail.png
rm /opt/sourcetrail -rf
update-mime/database /usr/share/mime
update-desktop-database

