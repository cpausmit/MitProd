#!/bin/bash

MIT_LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
BOOK=$1
FLAG=$2
version=`basename $BOOK`
first=1

for line in `list $MIT_LOCATION/$BOOK/\*/crab_0_\*`
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
    ssh paus@cgate.mit.edu rmdir $obsDir
    remove="1"
  fi

done
