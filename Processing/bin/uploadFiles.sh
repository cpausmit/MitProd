#!/bin/bash
#===================================================================================================
# Upload files $FIRST to $LAST from a given list of files. The list is per default set to files.txt.
#
#                                                                              C.Paus (Sep 30, 2015)
#===================================================================================================
# setting up
UPLOAD="uploadFile.sh"
FILELIST="./files.txt"
if ! [ -f "$FILELIST" ]
then
  echo ""
  echo " ERROR - filelist is not provided: $FILELIST"
  echo ""
  exit 0
fi

# say where we are
echo HOME: $HOME
echo FILES: *

if [ -f "uploadFile.sh" ]
then
  chmod 750 ./uploadFile.sh
  UPLOAD="./uploadFile.sh"
fi

# comand line parameters
FIRST="$1"
LAST="$2"

# show my file list with the corresponding numbers
echo ""
echo " My file list: $FILELIST"
echo ""
nl $FILELIST

# show the range we want to use
echo ""
echo " First:$FIRST  Last:$LAST"
echo ""

# loop through the requested files
next="$FIRST"
while [ $next -le $LAST ]
do

  file=`sed "${next}q;d" files.txt `
  if [ "$file" != "" ]
  then
    echo " $next:$file"
    echo " $UPLOAD $file"
    $UPLOAD $file
  else
    echo " Skip empty file: '$next:$file'"
  fi

  # increment to get next file
  next=$((next + 1))
done

exit 0
