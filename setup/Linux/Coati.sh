#!/bin/sh
appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
echo $dirname
tmp="${dirname#?}"
echo $tmp

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
echo $dirname
fi
LD_LIBRARY_PATH=$dirname/lib
export LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH

$dirname/$appname