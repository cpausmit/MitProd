#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Determining the status of a given production sample
#---------------------------------------------------------------------------------------------------
H=`basename $0`

BOOK=$1
DATASET=$2

DIR=/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus/$BOOK
if [ "`list $DIR | grep $DATASET`" != "" ]
then
  nAll=`wc -l $BOOK/$DATASET.lfns | cut -d ' ' -f 1`
  nDone=`list $DIR/$DATASET | grep root | wc -l`
  nMissing=$(( ${nAll}-${nDone} ))
  
  nEvents=`cat $BOOK/$DATASET.lfns | awk '{n=n+$3} END {print n}'`
  size=`list $DIR/$DATASET | grep root | awk '{s=s+$1} END {print s/1024./1024./1024.}'`
  sizePerEvent=`echo $nEvents $size | awk '{print $2*1024*1024/$1}'` 
  printf "%-14s %-30s %6d %6d %6d  Evts: %12d in %8.2f GB (%5.1f kB/evt)\n" \
         $BOOK $DATASET $nAll $nDone $nMissing $nEvents $size $sizePerEvent
fi

exit 0
