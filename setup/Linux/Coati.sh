#!/bin/sh
COATI_PATH=`dirname "$0"`

if [ ! -d "${HOME}/.config/coati" ]; then
  # Control will enter here if $DIRECTORY doesn't exist.
  cp -r $COATI_PATH/coati/data ${HOME}/.config/coati
  echo "First start of Coati from this user, copy default configfiles to ~/.config/coati"
fi

export LD_LIBRARY_PATH=$COATI_PATH/coati/lib
cd ~/.config/coati && exec $COATI_PAT/coati/Coati
