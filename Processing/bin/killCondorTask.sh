#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Remove all jobs related with a given task and sample.
#---------------------------------------------------------------------------------------------------
H=`basename $0`
TASK=$1
SAMPLE=$2
TIMEOUT=$3
EXEC=$4

# health checks
if [ ".$1" == "." ] || [ ".$2" == "." ]
then
  echo "";
  echo "usage: $H  <sample>  <task>";
  echo ""
  exit 1
fi

# say what we do
echo ""
echo " ==== Kill local condor jobs matching Sample=$SAMPLE and Task=$TASK ===="
echo ""

# loop through jobs matching the requirements
export idxs=""
condor_q $USER -format "%d " ClusterId -format "%d " ServerTime \
               -format "%d " EnteredCurrentStatus -format "%s " Cmd -format "%s\n " Args | \
  grep $TASK | grep $SAMPLE | \

while read line
do
  echo "Line: $line"
  id=`echo $line | tr -s ' ' | cut -d ' ' -f 1`
  runTime=`echo $line | tr -s ' ' | cut -d ' ' -f 2`
  tmp=`echo $line | tr -s ' ' | cut -d ' ' -f 3`
  runTime=$(( $runTime + $tmp ))
  dir=`echo $line | tr -s ' ' | cut -d ' ' -f 4`
  file=`echo $line | tr -s ' ' | cut -d ' ' -f 5`
  echo "$id $runTime --> $line"
  if [ $runTime -gt $TIMEOUT ]
  then
    echo " -> removing: condor_rm $id "
    if [ "$EXEC" == "exec" ]
    then
      condor_rm $id
      condor_rm -forcex $id
      echo "remove $dir/$file"
    fi
  fi
done
echo ""

exit 0
