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
jobs=`condor_q -global $USER -format "%s " Cmd -format "%s \n" Args | grep $dataset | grep downloadFiles.sh | wc -l`
while [ $jobs -gt $remainingMax ]
do
  echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"; sleep 60; echo ""
  jobs=`condor_q -global $USER -format "%s " Cmd -format "%s \n" Args | grep $dataset | grep downloadFiles.sh | wc -l`
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
  
  # download the sample
  downloadSample.sh $line
  
  # go into waiting loop
  nowTime=$(date +%s); duration=$(($nowTime - $startTime))
  jobs=`condor_q -global $USER -format "%s " Cmd -format "%s \n" Args | grep $dataset | grep downloadFiles.sh | wc -l`
  while [ $jobs -gt $remainingMax ]
  do
    echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"; sleep 60; echo ""
    jobs=`condor_q -global $USER -format "%s " Cmd -format "%s \n" Args | grep $dataset | grep downloadFiles.sh | wc -l`
    nowTime=$(date +%s); duration=$(($nowTime - $startTime))
  done
  echo " Queues are empty ($jobs) --> cleaning up and making catalogs."
  echo ""
  
  # remove zero length files
  echo "removeZeroLengthFiles.sh $targetDir/$book/$dataset"
  removeZeroLengthFiles.sh $targetDir/$book/$dataset
  
  # finally make the corresponding catalog
  if [ ${#version} == 3 ]
  then
    echo "catalog.sh -ceg $version $dataset --retry $targetDir"
    catalog.sh -ceg $version $dataset --retry $targetDir
  else
    echo "This is not a normal dataset, make a simple catalog."
    echo ""
    list $targetDir/$book/$dataset > /tmp/list.$$
    mkdir -p ~/catalog/t2mit/private/$book/$dataset
    rm -f    ~/catalog/t2mit/private/$book/$dataset/Files
    touch    ~/catalog/t2mit/private/$book/$dataset/Files
    while read line
    do
      size=`echo $line | cut -d ' ' -f1`
      file=`echo $line | cut -d ' ' -f2`
      root -l -b -q $MIT_PROD_DIR/root/runSimpleFileCataloger.C\(\"$targetDir/$book/$dataset\",\"$file\"\) \
	  2> /dev/null | grep CATALOG >> ~/catalog/t2mit/private/$book/$dataset/Files
    done < /tmp/list.$$
    rm -f /tmp/list.$$
  fi

  i=$(( $i+1 ))

done

exit 0
