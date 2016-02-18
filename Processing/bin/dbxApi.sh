#!/bin/bash
# ----
# Check number of dropbox upload jobs in condor queue and submit next sample
# once the number of jobs drops below the threshold
#
# Options: OPTIONS = ( size, copy, move ) 
#
# ----

[ -z "$BOOK" ]       && BOOK="filefi/043"
[ -z "$NTRANSFERS" ] && NTRANSFERS=5
[ -z "$THRESHOLD" ]  && THRESHOLD=10

# initialize other global variable
SLEEPING=300
# global variables
DROPBOX_SIZE=0

startTime=$(date +%s)
touch ~/DropboxUploadMonitor.data

function submitSample()
{
  sample="$1"  
  option="$2"  

  duration=0
  startTime=$(date +%s)
  nJobs=`condor_q $USER -l | egrep '^Cmd = .*uploadFiles.sh' | wc -l`

  # check there are less than $THRESHOLD jobs running otherwise wait
  while [ $nJobs -gt $THRESHOLD ]
  do
    echo " $nJobs condor jobs [ waiting since $duration sec ] next $sample"
    nowTime=$(date +%s)
    duration=$(($nowTime - $startTime))
    echo "sleep $SLEEPING"
    sleep $SLEEPING
    nJobs=`condor_q $USER -l | egrep '^Cmd = .*uploadFiles.sh' | wc -l`
  done

  # check first whether this samples is actually being worked on
  thisSample=`condor_q $USER -format "%s\n" Out | grep upload | grep $sample`

  # no more than $THRESHOLD jobs running, submit more (up to $NTRANSFERS)
  if [ "$thisSample" == "" ]
  then
    #echo " uploadSample.sh  /store/user/paus/$BOOK/$sample $NTRANSFERS " 
    uploadSample.sh  /store/user/paus/$BOOK/$sample $NTRANSFERS $option
    echo ""
  else
    echo " Sample is already being worked, skip...." 
    echo ""
  fi
}

function sizeSample()
{
  SAMPLE="$1"  
  sampleSize=0
  sizes=`~/Tools/Dropbox-Uploader/dropbox_uploader.sh list \
         /cms/store/user/paus/$BOOK/$SAMPLE | tr -s ' ' | cut -d ' ' -f3`
  for size in $sizes
  do
    sampleSize=$(( sampleSize + size ))
  done
  sampleSize=$(( sampleSize / 1000000 ))
  DROPBOX_SIZE=$((DROPBOX_SIZE + sampleSize))
  #printf " Adding [MB]: %7d  --> %9d -- %s\n" $sampleSize $DROPBOX_SIZE $SAMPLE
}

#---------------------------------------------------------------------------------------------------
# M A I N  -- main action starts
#---------------------------------------------------------------------------------------------------

MODE="$1"

# get the done sample list if needed
done=""
if [ "$MODE" != "size" ] && [ "$MODE" != "copy" ] &&  [ "$MODE" != "move" ]
then
  done=`~/Tools/Dropbox-Uploader/dropbox_uploader.sh list /cms/store/user/paus/$BOOK \
        | tr -s ' ' | cut -d ' ' -f4`
fi

# acquire the list of samples to deal with
sampleList=`list /mnt/hadoop/cms/store/user/paus/$BOOK | cut -d' ' -f2`
if [ "$MODE" == "size" ]
then
  sampleList=`~/Tools/Dropbox-Uploader/dropbox_uploader.sh list /cms/store/user/paus/$BOOK \
              | tr -s ' ' | cut -d ' ' -f4`
fi

# loop over all samples
for sample in $sampleList
do

  if   [ "$MODE" == "size" ]
  then
    sizeSample $sample
  elif [ "$MODE" == "copy" ]
  then
    echo " NEXT - $sample"
    submitSample $sample
  elif [ "$MODE" == "move" ]
  then
    echo " NEXT - $sample"
    submitSample $sample remove
  elif [ "`echo $done | grep $sample`" != "" ]
  then
    echo " NEXT - $sample"
    echo " Sample already present. Next"
    echo ""
  else
    echo " NEXT - $sample"
    submitSample $sample
  fi

done

if [ "$MODE" == "size" ]
then
  nowTime=$(date +%s)
  duration=$(($nowTime - $startTime))

  totalTb=$(( DROPBOX_SIZE / 1000 ))
  printf "\n TOTAL [GB]: %6d (%d sec)\n\n" $totalTb $duration

  # add to out monitoring file  
  printf " %6d %10d %10d\n" $totalTb $startTime $nowTime >> ~/DropboxUploadMonitor.data
fi

exit 0
