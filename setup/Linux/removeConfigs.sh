
MY_PATH=`dirname "$0"`

echo "Are you sure to delete your coait configs"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

rm -rf ~/.config/coati

