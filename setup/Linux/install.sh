
MY_PATH=`dirname "$0"`
MY_PATH=../$MY_PATH 

cat EULA.txt
echo "Agree to the EULA"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

echo "Run this script as root"

ln -s $MY_PATH/Coati.sh /usr/bin/Coati
cp $MY_PATH/setup/application-x-coatiproject /usr/share/mime/packages
cp $MY_PATH/setup/coati.desktop /usr/share/applications/
cp $MY_PATH/data/gui/icon/logo_1024_1024.png /usr/share/icon/coati.png
cp $MY_PATH/data/gui/icon/project_256_256.png /usr/share/icons/project-coati.png
update-mime-database /usr/share/mime

