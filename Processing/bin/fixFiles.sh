#!/bin/bash
#===================================================================================================
# Assume a list of local files is found to be broken. Go through the list and recover one by one.
#===================================================================================================
FILE="$1"
FLAG="$2"

while read line
do

  fullFile=`echo $line | cut -d ' ' -f1`
  fixFile.sh $fullFile $FLAG

done < $FILE
