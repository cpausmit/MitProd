#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Monitor progress of download into a given directory
#---------------------------------------------------------------------------------------------------
H=`basename $0`

## DIR=/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus/filefi/015
DIR=$1
PATTERN=$2

# total size and starting point
startTime=$(date +%s)
totSize=`du -s --block-size=1G $DIR | cut -f1`
startSize=$totSize
lastSize=$totSize
lastTime=$(date +%s);

while [ 1 -ge 0 ]
do

  nowTime=$(date +%s);
  totSize=`du -s --block-size=1G $DIR | cut -f1`

  lastDuration=$(($nowTime - $lastTime))
  lastAdded=$(($totSize - $lastSize))

  duration=$(($nowTime - $startTime))
  totAdded=$(($totSize - $startSize))

  if [ $duration -gt 0 ] && [ $lastDuration -gt 0 ]
  then
    totRate=` echo h | awk "{ print $totAdded/$duration }"`
    lastRate=`echo h | awk "{ print $lastAdded/$lastDuration }"`
  else
    totRate=0
    lastRate=0
  fi
  clear
  date=`date +"Refresh 100 sec -- last update  %H:%M:%S [%d/%m/%y]"`
  lCols=`stty size | cut -d' ' -f2`
  printf "%${lCols}s" "$date"
  echo " "
  echo "==== Transfer rate to $DIR ===="
  echo " "
  if [ "$PATTERN" == "" ]
  then
    du -sh $DIR/* | tail -44
  else
    du -sh $DIR/* | grep $PATTERN
  fi
  echo " "
  printf "==== Size [GB]    : %6d (last: %6d) ====\n" $totSize $lastSize
  printf "==== Rate [GB/sec]: %6.3f (last: %6.3f) ====\n" $totRate $lastRate
  #echo "==== Size [GB]    : $totSize (last: $lastSize) ===="
  #echo "==== Rate [GB/sec]: $totRate (last: $lastRate) ===="
  echo " "
  #echo "== $duration - $lastDuration =="

  # update last status
  lastSize=$totSize
  lastTime=$nowTime

  # finally wait a bit
  sleep 100

done

exit 0
