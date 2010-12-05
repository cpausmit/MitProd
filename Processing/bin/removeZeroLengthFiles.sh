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

ls -1l $DIR | grep root | \
while read line
do
  size=`echo $line | tr -s ' ' | cut -d ' ' -f 5`
  file=`echo $line | tr -s ' ' | cut -d ' ' -f 9`
  if [ "$size" == "0" ]
  then
    echo "File: delete $file with $size bytes"
    rm $DIR/$file
  #else
  #  echo "File: -keep- $file with $size bytes"
  fi
done
