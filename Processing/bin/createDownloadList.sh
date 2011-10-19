#!/bin/bash
#===================================================================================================
# Create a list of datasets that completely define a download on the basis of a number of
# parameters.
#
#                                                                             Ch.Paus, Aug 30, 2011
#===================================================================================================
MIT1_LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
MIT2_LOCATION="/mnt/hadoop/cms/store/user/paus"
CERN_LOCATION="/castor/cern.ch/user/p/paus"

BOOK=$1
PATTERN=$2
version=`basename $BOOK`

LOCATION="$MIT1_LOCATION/$BOOK $MIT2_LOCATION/$BOOK"

echo "list $MIT1_LOCATION/$BOOK"
list $MIT1_LOCATION/$BOOK > /tmp/datasets-1-$$
echo "list $MIT2_LOCATION/$BOOK"
list $MIT1_LOCATION/$BOOK > /tmp/datasets-2-$$

while read line
do
  if [ "`echo $line | grep $PATTERN`" != "" ]
  then
    dset=`echo $line | cut -d' ' -f2`
    printf "/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus filefi/023 %30s /mnt/hadoop/cmsprod 80 /home/cmsprod/download\n" $dset
  fi

done < /tmp/datasets-1-$$

while read line
do
  if [ "`echo $line | grep $PATTERN`" != "" ]
  then
    dset=`echo $line | cut -d' ' -f2`
    printf "/mnt/hadoop/cms/store/user/paus               filefi/023 %30s /mnt/hadoop/cmsprod 80 /home/cmsprod/download\n" $dset
  fi

done < /tmp/datasets-1-$$

#/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus filefi/023    r11b-dmu-pr-v1 /mnt/hadoop/cmsprod 80 /home/cmsprod/download


  #if [ "`echo $line | grep :`" != "" ]
  #then
  #  dir=`echo $line | tr -d :`
  #  obsDir=$dir
  #  remove="0"
  #
  #  #echo $line
  #  crabId=`basename $dir`
  #  dir=`dirname $dir`
  #  dataset=`basename $dir`
  #  extDataset="$dataset/$crabId"
  #  first=1
  #fi
  #
  #if [ "$first" == 1 ] && [ "`echo $line | grep root`" != "" ]
  #then
  #  echo "catalog.sh -ceg $version $extDataset --remove"
  #  catalog.sh -ceg $version $extDataset --remove
  #  first=0
  #fi 
  #
  #if [ "$remove" == "0" ] && [ "$FLAG" == "remove" ]
  #then
  #  echo "removing: $obsDir"
  #  ssh paus@cgate.mit.edu rmdir $obsDir
  #  remove="1"
  #fi

#done
