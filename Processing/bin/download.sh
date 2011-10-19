#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Go through a number of download requests
#---------------------------------------------------------------------------------------------------
H=`basename $0`
SAMPLE_LIST=$1

# start next request
remainingMax=0

# health checks
if [ ".$1" == "." ]
then
  echo "";
  echo "usage: $H  <sampleList>";
  echo ""
  exit 1
fi

# say what we do
echo ""
echo " ==== Download queue from $SAMPLE_LIST ===="
echo ""

# check that queues are really empty
dataset="XXX"
startTime=$(date +%s)
nowTime=$(date +%s); duration=$(($nowTime - $startTime))
jobs=`condor_q -global $USER -format "%d " ClusterId -format "%s " Cmd -format "%s \n" Args | grep $dataset | grep downloadFiles.sh | wc -l`
while [ $jobs -gt $remainingMax ]
do
  echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"; sleep 60; echo ""
  jobs=`condor_q -global $USER $format | grep $dataset | grep downloadFiles.sh | wc -l`
  #jobs=`condor_q -global $USER | grep downloadFiles.sh | wc -l`
  nowTime=$(date +%s); duration=$(($nowTime - $startTime))
done
echo " Queues are close to empty ($jobs) -- Let's get started."
echo ""

# all right, ready to download
nSamples=`cat $SAMPLE_LIST | wc -l | cut -d ' ' -f1`
i=1
while [ $i -le $nSamples ]
do
  startTime=$(date +%s)

  line=`head -$i $SAMPLE_LIST | tail -1`
  if [ "`echo $line | grep ^#`" != "" ]
  then
    i=$(( $i+1 ))
    continue
  fi

  # define some quantities
  baseDir=`echo $line | tr -s ' ' | cut -d ' ' -f 1`
  book=`echo $line | tr -s ' ' | cut -d ' ' -f 2`
  version=`basename $book`
  dataset=`echo $line | tr -s ' ' | cut -d ' ' -f 3`
  targetDir=`echo $line | tr -s ' ' | cut -d ' ' -f 4`

  # stagein the sample if it is at CERN
  if [ "`echo $baseDir | grep /castor/cern.ch`" != "" ]
  then
    echo "  ssh paus@lxplus.cern.ch ./stageSample.py --dataDir=$baseDir/$book/$dataset"
    ssh paus@lxplus.cern.ch ./stageSample.py --dataDir=$baseDir/$book/$dataset
  fi

  # download the sample
  echo " downloadSample.sh $line"
  downloadSample.sh $line
  
  # go into waiting loop
  nowTime=$(date +%s); duration=$(($nowTime - $startTime))
  jobs=`condor_q -global $USER -format "%d " ClusterId -format "%s " Cmd -format "%s \n" Args | grep $dataset | grep downloadFiles.sh | wc -l`
  while [ $jobs -gt $remainingMax ]
  do
    echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"; sleep 60; echo ""
    jobs=`condor_q -global $USER -format "%d " ClusterId -format "%s " Cmd -format "%s \n" Args | grep $dataset | grep downloadFiles.sh | wc -l`
    nowTime=$(date +%s); duration=$(($nowTime - $startTime))
  done
  echo " Queues are empty ($jobs) --> cleaning up and making catalogs."
  echo ""
  
  # remove zero length files
  echo "removeZeroLengthFiles.sh $targetDir/$book/$dataset"
  removeZeroLengthFiles.sh $targetDir/$book/$dataset
  
  # finally make the corresponding catalog
  echo "catalog.sh -ceg $version $dataset --retry $targetDir"
  catalog.sh -ceg $version $dataset --retry $targetDir

  i=$(( $i+1 ))

done

exit 0
