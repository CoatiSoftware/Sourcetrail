SCRIPT_DIR="$( cd "$( dirname "$0" )" && pwd )"
MY_PATH=$SCRIPT_DIR/..

cat ${MY_PATH}/EULA.txt
echo "Agree to the EULA"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

echo "Run this script as root"

cp -rf $MY_PATH/ /opt/sourcetrail/ > /dev/null
cp $MY_PATH/setup/sourcetrail-mime.xml /usr/share/mime/packages > /dev/null
cp $MY_PATH/setup/sourcetrail.desktop /usr/share/applications/ > /dev/null
cp $MY_PATH/data/gui/icon/logo_1024_1024.png /usr/share/icons/sourcetrail.png > /dev/null
cp $MY_PATH/data/gui/icon/project_256_256.png /usr/share/icons/project-sourcetrail.png > /dev/null
update-mime-database /usr/share/mime > /dev/null
update-desktop-database > /dev/null

ln -f -s /opt/sourcetrail/Sourcetrail.sh /usr/bin/sourcetrail > /dev/null

