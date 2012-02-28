#!/bin/bash
#===================================================================================================
# Spawn download activities to optimize the bandwidth of the download process, one step closer to
# Phedex.
# 
#                                                                                   C.Paus, Nov 2011
#---------------------------------------------------------------------------------------------------
# Improvements to implement on a rainy Friday afternoon:
#  + connect back to an existing task
#  - optimize sleep time to allow processes to get to submit their missing file requests
#  - allow for priorities
#===================================================================================================

#---------------------------------------------------------------------------------------------------
findActivity()
{
  condor_q -global $USER -format "%d " ClusterId -format "%s " Cmd -format "%s \n" Args \
    | grep downloadFiles.sh | wc -l
}

#---------------------------------------------------------------------------------------------------
showAccounting()
{
  pid=$1; shift; iDownload=$1; shift; nDownloads=$1; shift; nJobs=$1; shift; nProcs=$1; shift
  echo " "
  echo " spawnDownload.sh  ==== Accounting Summary ==== "
  echo " "
  echo " Master pid         : $pid "
  echo " Last download tasks: $iDownload / $nDownloads "
  echo " Running processes  : $nProcs "
  echo " Running batch      : $nJobs "
  echo " "
}

#---------------------------------------------------------------------------------------------------
showAccountingShort()
{
  pid=$1; shift; iDownload=$1; shift; nDownloads=$1; shift; nJobs=$1; shift; nProcs=$1; shift
  echo " Downloads ($pid): $iDownload / $nDownloads  nProcs: $nProcs   nJobs: $nJobs "
}

#---------------------------------------------------------------------------------------------------
loopCondition()
{
  # command line option
  iDownload=$1; shift; nDownloads=$1; shift; nJobs=$1; shift;
  # return running condition
  if [ $iDownload -lt $nDownloads ] || [ $nJobs -gt 0 ]
  then
    echo 'true'
  else
    echo 'false'
  fi
}

#---------------------------------------------------------------------------------------------------
spawnCondition()
{
  # command line option
  iDownload=$1; shift; nDownloads=$1; shift; nJobs=$1; shift; nProcs=$1; shift
  # return running condition
  if [ $iDownload -lt $nDownloads ] && [ $nProcs -lt $NPROC_MAX ] && [ $nJobs -lt $NJOBS_MAX ]
  then
    echo 'true'
  else
    echo 'false'
  fi
}

#---------------------------------------------------------------------------------------------------
addProcess()
{
  pidFile=$1; shift; logFile=$1; shift;
  commandLine=$*
  $commandLine >& $logFile &
  pid=$!
  # put some useful information into the pid file
  file=`echo $commandLine | cut -d' ' -f2`
  download=`cat $file`
  echo $pid :: $commandLine :: $download >> $pidFile
  echo " Spawning pid: $pid ::  $commandLine :: $download"
}

#---------------------------------------------------------------------------------------------------
cleanUpPids()
{
  # read parameter
  pidFile=$1; shift
  # prepare empty temporary pid file
  rm -f $pidFile.tmp
  touch $pidFile.tmp
  echo " Cleaning Pids..."
  
  # parse the exisiting pid file

  while read line
  do
  
    pid=`echo $line | cut -d ' ' -f1`

    kill -0 $pid >& /dev/null
    if [ "$?" == "0" ]
    then
      grep ^$pid $pidFile >> $pidFile.tmp
    else
      grep ^$pid $pidFile >> $pidFile.done
    fi
  
  done < $pidFile

  # overwrite the existing with the active pids
  mv $pidFile.tmp $pidFile
}

#===================================================================================================
# Main activity starts here
#===================================================================================================
# defaults
LOGDIR=/home/cmsprod/public_html/download
# maximum number of processes and jobs
if [ "$NPROC_MAX" == "" ] 
then
  NPROC_MAX=8
else
  echo " Using environment NPROC_MAX: $NPROC_MAX"
fi
if [ "$NJOBS_MAX" == "" ] 
then
  NJOBS_MAX=80
else
  echo " Using environment NJOBS_MAX: $NJOBS_MAX"
fi
if [ "$SLEEP" == "" ] 
then
  SLEEP=30
else
  echo " Using environment SLEEP: $SLEEP"
fi

echo " "
echo " spawnDownload.sh  ==== Configuration ==== "
echo " "
echo " Maximal Jobs       : $NJOBS_MAX "
echo " Maximal Processes  : $NPROC_MAX "
echo " Sleeping time      : $SLEEP "
echo " "

# read command line arguments
INPUT_FILE=$1
if [ "$2" != "" ]
then
  MASTER_PID=$2
else
  MASTER_PID=$$
fi

# prepare pid file
pidFile=$LOGDIR/pids.$MASTER_PID
if [ "$2" == "" ]
then
  rm -f $pidFile
  touch $pidFile
  iDownload=0
else
  iDownload=`ls $LOGDIR/download.$MASTER_PID.* -1|cut -d'.' -f3|awk '{ if ($1 > n) n=$1; } END{print n}'`
fi

# prepare work accounting
nDownloads=`cat $INPUT_FILE | grep -v ^# | wc -l`
nJobs=`findActivity`
nProcs=`cat $pidFile | wc -l`
showAccounting $MASTER_PID $iDownload $nDownloads $nJobs $nProcs

startTime=$(date +%s)
nowTime=$(date +%s); duration=$(($nowTime - $startTime))

# the loop which finishes when activity has ceased
while [ "`loopCondition $iDownload $nDownloads $nJobs`" == "true" ]
do

  # short message we are still in the loop
  nowTime=$(date +%s); duration=$(($nowTime - $startTime))
  echo " "
  echo " Loop continues... (spawnDownload.sh: duration - $duration)"
  sleep $SLEEP

  # are we spawning a new process?
  if [ "`spawnCondition $iDownload $nDownloads $nJobs $nProcs`" == "true" ]
  then
    iDownload=$(( $iDownload + 1 ))
    #echo " Download file: $INPUT_FILE - line $iDownload ($nDownloads)"
    cat $INPUT_FILE | grep -v ^# | sed -n ${iDownload}p  > $LOGDIR/download.$MASTER_PID.$iDownload
    addProcess $pidFile $LOGDIR/download.$MASTER_PID.$iDownload.log download.sh \
                        $LOGDIR/download.$MASTER_PID.$iDownload
  fi

  # full accounting step
  cleanUpPids $pidFile 
  # get updated counts on batch jobs and number of processes
  nJobs=`findActivity`
  nProcs=`cat  $pidFile | wc -l`
  echo " ==== Last update before decision ==== "
  showAccountingShort $MASTER_PID $iDownload $nDownloads $nJobs $nProcs

  makeDownloadIndex.sh
  #cp $pidFile $LOGDIR/download.$MASTER_PID.*.log ~/public_html/download/

done

# cleanup the steering files
rm -f $pidFile $pidFile.tmp

exit 0;
