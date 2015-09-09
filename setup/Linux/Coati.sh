#!/bin/sh
if [ ! -d "${HOME}/.config/coati" ]; then
  # Control will enter here if $DIRECTORY doesn't exist.
  cp -r /usr/local/share/coati/.config/coati ${HOME}/.config/coati
  echo "First start of Coati from this user, copy default configfiles to ~/.config/coati"
fi

export LD_LIBRARY_PATH=/usr/local/share/coati/lib
cd ~/.config/coati && exec /usr/local/share/coati/Coati
