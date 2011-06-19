#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Determining the status of a given production sample
#---------------------------------------------------------------------------------------------------
H=`basename $0`

CATALOG=$HOME/catalog
BOOK=$1
DATASET=$2
PRINTOPT=$3

if [ "$PRINTOPT" == "-header" ]
then
  date=`date`
  printf "\nUpdated: $date\n\n%s\n" \
    'BOOK        SAMPLE                          All     T2(Catalg)   ToDo       T3(Catalg) ToDoT3'
  printf "%s\n" \
    '---------------------------------------------------------------------------------------------'
fi

DIR=/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus/$BOOK
T3DIR=/mnt/hadoop/cmsprod/$BOOK
if [ "`list $DIR | grep $DATASET`" != "" ]
then
  nAll=`wc -l $BOOK/$DATASET.lfns | cut -d ' ' -f 1`
  # how many are done on the Tier-2
  nDone=`list $DIR/$DATASET 2> /dev/null | grep root | wc -l`
  # how many are done on the Tier-2
  nCata=`cat $CATALOG/t2mit/$BOOK/$DATASET/Files 2> /dev/null | grep -v ^# | wc -l`
  # how many are missing on the Tier-2
  nMissing=$(( ${nAll}-${nDone} ))
  # how many are available on the Tier-3
  nDoneT3=`list $T3DIR/$DATASET 2> /dev/null | grep root | wc -l`
  # how many are done on the Tier-3
  nCataT3=`cat $CATALOG/local/$BOOK/$DATASET/Files 2> /dev/null | grep -v ^# | wc -l`
  # how many are missing on the Tier-3
  nMissingT3=0
  if [ $nDoneT3 -gt 0 ]
  then
    nMissingT3=$(( ${nDone}-${nDoneT3} ))
  fi
  # event number, sample and event sizes
  nEvents=`cat $BOOK/$DATASET.lfns | awk '{n=n+$3} END {print n}'`
  size=`list $DIR/$DATASET 2> /dev/null | grep root | awk '{s=s+$1} END {print s/1024./1024./1024.}'`
  sizePerEvent=`echo $nEvents $size | awk '{print $2*1024*1024/$1}'` 
  # print the result
  printf "%-11s %-28s %6d %6d(%6d) %6d - %6d(%6d) %6d %12d ev, %8.2f GB, %5.1f kB/ev\n" \
    $BOOK $DATASET \
    $nAll $nDone $nCata $nMissing $nDoneT3 $nCataT3 $nMissingT3 $nEvents $size $sizePerEvent
fi

# if [ $nDone -gt $nCata ]
# then
#   echo "catalog.sh -ceg `basename $BOOK` $DATASET --retry"
#   catalog.sh -ceg `basename $BOOK` $DATASET --retry
# fi

exit 0
