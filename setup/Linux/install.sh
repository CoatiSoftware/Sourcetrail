
MY_PATH=`dirname "$0"`
MY_PATH=../$MY_PATH

cat ${MY_PATH}/EULA.txt
echo "Agree to the EULA"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

echo "Run this script as root"

cp -rf $MY_PATH/ /opt/coati/ > /dev/null
cp $MY_PATH/setup/coati-mime.xml /usr/share/mime/packages > /dev/null
cp $MY_PATH/setup/coati.desktop /usr/share/applications/ > /dev/null
cp $MY_PATH/data/gui/icon/logo_1024_1024.png /usr/share/icons/coati.png > /dev/null
cp $MY_PATH/data/gui/icon/project_256_256.png /usr/share/icons/project-coati.png > /dev/null
update-mime-database /usr/share/mime > /dev/null
update-desktop-database > /dev/null

ln -f -s /opt/coati/Coati.sh /usr/bin/coati > /dev/null

