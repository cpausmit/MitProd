#!/bin/bash

DIR=$1

# Health checks
if [ ".$1" == "." ]
then
  echo "";
  echo "usage: $H  <sampleList>";
  echo ""
  exit 1
fi

list $DIR | grep root | \
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
