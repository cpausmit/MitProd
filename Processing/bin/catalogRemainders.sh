#!/bin/bash
#===================================================================================================
# Remove leftover directories from older productions, trying to recover the files and possibly
# deleting them.
#===================================================================================================
MIT_LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
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
    delete="0"

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

  if [ "$delete" == "0" ] && [ "$FLAG" == "delete" ]
  then
    echo "deleting: $obsDir"
    ssh paus@cgate.mit.edu rm -rf $obsDir
    delete="1"
  fi

done
