#!/bin/bash
#===================================================================================================
# Call file check on hadoop and store the corrupted files and files with missing blocks.
#===================================================================================================

# read command line parameters
BROKEN_FILE_LIST="broken.txt"
if [ "$1" != "" ]
then
  BROKEN_FILE_LIST="$1"
fi
rm -f $BROKEN_FILE_LIST
touch $BROKEN_FILE_LIST

DEBUG=""
if [ "$2" != "" ]
then
  DEBUG="$1"
fi

# make a file with the fsck output
hdfs fsck / > /tmp/hadoop-fsck.$$

# loop over the hadoop fsck output and find relevant files
while read line
do

  if [ "`echo $line | egrep \(CORRUPT\|MISSING\)`" != "" ] &&
     [ "`echo $line | grep ^/cmsprod`" != "" ]
  then
    fullFile=`echo $line | cut -d ':' -f1 | sed "s#^#/mnt/hadoop#"`
    if [ "$DEBUG" != "" ]
    then
      echo "LINE: $line"
      echo "$fullFile"
      echo ""
    fi
    echo "$fullFile" >> $BROKEN_FILE_LIST
  fi

done < /tmp/hadoop-fsck.$$
