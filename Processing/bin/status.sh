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

DIR1=/mnt/hadoop/cms/store/user/paus/$BOOK
T3DIR=/mnt/hadoop/cmsprod/$BOOK
if [ "`list $DIR1 2> /dev/null | grep $DATASET`" != "" ]
then
  #echo wc -l $BOOK/$DATASET.lfns '2> /dev/null | cut -d ' ' -f 1'
  if ! [ -e "$BOOK/$DATASET.lfns" ]
  then
    echo " Warning: complete file list not available. Creating it now."
    echo " -> findSamples.py --cmssw=mc --pattern=$DATASET --remakeLfns=1 --complete --exe"
    findSamples.py --cmssw=mc --pattern=$DATASET --remakeLfns=1 --complete --exe >& /dev/null
  fi
  nAll=`wc -l $BOOK/$DATASET.lfns 2> /dev/null | cut -d ' ' -f 1`
  # how many are done on the Tier-2
  #echo list $DIR1/$DATASET '2> /dev/null | grep root | wc -l'
  nDone=`list $DIR1/$DATASET 2> /dev/null | grep root | wc -l`
  # how many are done on the Tier-2
  #echo cat $CATALOG/t2mit/$BOOK/$DATASET/Files '2> /dev/null | grep -v ^# | wc -l'
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
  nEvents=`cat $BOOK/$DATASET.lfns 2> /dev/null | awk '{n=n+$3} END {print n}'`
  size=0
  if [ "$nEvents" != "" ] && [ "`list $DIR1/$DATASET 2> /dev/null`" != ""  ]
  then
    size=`list $DIR1/$DATASET 2> /dev/null | grep root | awk '{s=s+$1} END {print s/1024./1024./1024.}'`
  fi
  sizePerEvent=0
  if [ "$nEvents" != "" ] && [ $nEvents -gt 0 ]
  then
    sizePerEvent=`echo $nEvents $size | awk '{print $2*1024*1024/$1}'` 
  fi

  # print the result
  #echo " ECHO -- " \
  #  $BOOK $DATASET \
  #  $nAll $nDone $nCata $nMissing $nDoneT3 $nCataT3 $nMissingT3 $nEvents $size $sizePerEvent
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
