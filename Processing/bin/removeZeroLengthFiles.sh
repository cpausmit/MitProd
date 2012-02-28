#!/bin/bash

DIR=$1
if [ "$2" != "" ]
then
  PATTERN=$2
else
  PATTERN=root
fi

# Health checks
if [ ".$1" == "." ]
then
  echo "";
  echo "usage: $0  <directory-list> [ <pattern> = root ] ";
  echo ""
  exit 1
fi

list $DIR | grep $PATTERN | \
while read line
do

  size=`echo $line | tr -s ' ' | cut -d ' ' -f 1`
  file=`echo $line | tr -s ' ' | cut -d ' ' -f 2`
  if [ "$size" == "0" ]
  then
    echo "File: delete $file with $size bytes"
    remove --exe $DIR/$file
  fi

done
