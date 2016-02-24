
MY_PATH=`dirname "$0"`
MY_PATH=../$MY_PATH 

echo "Remove coati form system"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

echo "Run this script as root"

rm /usr/bin/Coati
rm /usr/share/mime/packages/application-x-coatiproject
rm /usr/share/applications/coati.desktop
rm /usr/share/icons/coati.png
rm /usr/share/icons/project-coati.png
update-mime/database /usr/share/mime

