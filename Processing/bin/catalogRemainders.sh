#!/bin/bash

MIT_LOCATION="/mnt/hadoop/cms/store/user/paus"
CERN_LOCATION="/castor/cern.ch/user/p/paus"
LOCATION=$MIT_LOCATION

BOOK=$1
FLAG=$2
version=`basename $BOOK`
first=1

for line in `list $LOCATION/$BOOK/\*/crab_0_\*`
do
  if [ "`echo $line | grep :`" != "" ]
  then
    dir=`echo $line | tr -d :`
    obsDir=$dir
    remove="0"

    #echo $line
    crabId=`basename $dir`
    dir=`dirname $dir`
    dataset=`basename $dir`
    extDataset="$dataset/$crabId"
    first=1
  fi

  if [ "$first" == 1 ] && [ "`echo $line | grep root`" != "" ]
  then
    echo "catalog.sh -ceg $version $extDataset --remove"
    catalog.sh -ceg $version $extDataset --remove
    first=0
  fi 

  if [ "$remove" == "0" ] && [ "$FLAG" == "remove" ]
  then
    echo "removing: $obsDir"
    glexec rmdir $obsDir
    remove="1"
  fi

done
