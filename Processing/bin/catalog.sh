#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Perform a number of cataloging action on a bunch of datasets
#---------------------------------------------------------------------------------------------------
H=`basename $0`
timeOut=400

catalog=0
extract=0
generate=0
nFilesPerSet=20
test=0
cataDir=$HOME/catalog
mitCfg=filefi
while getopts "cegtd:m:n:" o
do case "$o" in
  c) catalog=1;;
  e) extract=1;;
  g) generate=1;;
  t) test=1;;
  d) cataDir=$OPTARG;;
  m) mitCfg=$OPTARG;;
  n) nFilesPerSet=$OPTARG;;
  [?]) echo "usage: $H [-cegt] [-d <catalogDir> ] [-m <mitcfg> ] [-n <nEvts> ] <version> [ <pattern> [ <location> ] ]"
       exit 1;;
  esac
done
let "shiftIdx = $OPTIND - 1"
shift $shiftIdx

MIT_LOCATION="/mnt/hadoop/cms/store/user/paus"
CERN_LOCATION="/castor/cern.ch/user/p/paus"
# Health checks
if [ ".$1" == "." ]
then
  echo "";
  echo "usage: $H [-cegt] [-m <mitcfg> ] [-n <nEvts> ] <version> [ <pattern> [ <location> ] ]";
  echo ""
  exit 1
fi
# Decode command line parameters
VERSION="$1"
PATTERN="empty"
if [ ".$2" != "." ]
then
  PATTERN=$2
fi
OPTION=""
if [ ".$3" != "." ]
then
  OPTION="$3"
fi
if [ ".$4" != "." ]
then
  LOCATION=$4
fi

# Conclude what needs to be done
if [ "`echo $LOCATION | grep /castor/cern.ch/`" == "" ] && \
   [ "`echo $LOCATION | grep /mnt/hadoop/cms/store/`" == "" ] && \
   [ "$LOCATION" != "" ]
then
  cataDir=$cataDir/local
  #cataDir=$cataDir/local/` basename \`dirname $LOCATION\` `/`basename $LOCATION`
elif [ ".`echo $HOSTNAME | grep cern.ch`" != "." ]
then
  LOCATION=$CERN_LOCATION
  cataDir=$cataDir/cern
elif [ ".`echo $HOSTNAME | grep mit.edu`" != "." ]
then
  LOCATION=$MIT_LOCATION
  cataDir=$cataDir/t2mit
fi

# Is it a skim?
SKIM=""
addSkim=""
if [ "`echo $LOCATION | grep /cmsprod/skim/`" != "" ]
then
  SKIM=`dirname $LOCATION`
  SKIM=`basename $SKIM`
  addSkim="/$SKIM"
fi

# Create a list of the datsets we need to catalog
LIST=`list ${LOCATION}/$mitCfg/$VERSION | cut -d ' ' -f2`
if   [ "`echo $PATTERN | grep crab_0_`" != ""  ]
then
  LIST="${LOCATION}/$mitCfg/$VERSION/$PATTERN"
fi

#---------------------------------------------------------------------------------------------------
# In the following we are looking at data files in
#
#   $LOCATION/$mitCfg/$VERSION/*$PATTERN*                         (defaults are at Tier-2 or castor)
#
# They are going to be cataloged and entries are created at
#
#   $cataDir (location of the catalog)
#   in book:                         $mitCfg/$VERSION
#   in dataset matching the pattern: $PATTERN
#
# For skims
#
#   $cataDir (location of the catalog)
#   in book:                         $mitCfg/$VERSION/$DATASET/$SKIM
#   in dataset matching the pattern: $PATTERN
#---------------------------------------------------------------------------------------------------
# Loop through the list and catalog --> ACTION STARTS BELOW HERE
#---------------------------------------------------------------------------------------------------
for dataset in $LIST
do
  dir=`dirname $dataset`
  dataset=`basename $dataset`
  #echo "DIR: $dir  DATASET: $dataset"

  # Extend dataset name for official dataset
  if [ "`echo $dataset | grep crab_0_`" != ""  ]
  then
    crabId=$dataset
    extDataset=`basename $dir`/$dataset
    dataset=`basename $dir`

    # Show present status
    lfnFile="$HOME/cms/jobs/$mitCfg/$VERSION/${dataset}.lfns_$crabId"
    nProc=`list $dir/$crabId | grep root | wc -l`
    nCata=`list $dir         | grep root | wc -l`
    nTota=`cat $lfnFile                  | wc -l`
    printf " CATALOG - Lfns: %6d  (done)  %6d  (to catalog) / %6d (total: %s)\n" \
	   $nCata $nProc $nTota $mitCfg/$VERSION/${dataset}.lfns_$crabId

    if [ "$nProc" == 0 ] && [ "$catalog" == 1 ]
    then
      echo ' nothing to catalog, moving on.'
      echo ' '
      continue
    fi

  else
    extDataset=$dataset
  fi

  # Break it off here in case we are only testing
  if [ $test != 0 ]
  then
    continue
  fi

  if [ "$PATTERN" == "empty" ] || [ "`echo $extDataset | grep $PATTERN`" != "" ]
  then
    #-----------------------------------------------------------------------------------------------
    # Test and potentially cleanup the dataset information
    #-----------------------------------------------------------------------------------------------
    extractCatalog.py --compact --catalog=$cataDir --mitCfg=$mitCfg --version=$VERSION \
                      --dataset=$extDataset${addSkim}
   
    #-----------------------------------------------------------------------------------------------
    # Submit the cataloging jobs
    #-----------------------------------------------------------------------------------------------
    if   [ "$catalog" == 1 ]
    then
      catalogFiles.sh   $LOCATION $cataDir $mitCfg/$VERSION $extDataset
      echo ""
    fi

    # start waiting time now
    startTime=$(date +%s)

    # Figure out whether there are new files to consider
    newFiles=0
    if [ "`ls -1 $cataDir/condor/$mitCfg/$VERSION/$extDataset${addSkim} | grep root`" != "" ]
    then
      newFiles=1
    fi

    #-----------------------------------------------------------------------------------------------
    # Extract the catalog metadata
    #-----------------------------------------------------------------------------------------------
    if [ "$extract" == 1 ] && ( [ "$newFiles" == 1 ] || [ "$catalog" != 1 ] )
    then
      nowTime=$(date +%s)
      duration=$(($nowTime - $startTime))
      jobs=`condor_q -global $USER -l | egrep '^Args.*root' | grep $extDataset | wc -l`
      while [ "$jobs" != "0" ]
      do
	echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"
	sleep 10
	echo ""
	jobs=`condor_q -global $USER -l | egrep '^Args.*root' | grep $extDataset | wc -l`
        nowTime=$(date +%s)
        duration=$(($nowTime - $startTime))
        if [ $duration -gt $timeOut ]
        then
          killCondorTask.sh catalog $extDataset $timeOut exec
        fi
      done
      echo " Queues are empty ($jobs) -->  moving on and extract cataloging results."
      echo ""
      extractCatalog.py $OPTION \
        --catalog=$cataDir --mitCfg=$mitCfg --version=$VERSION --dataset=$extDataset${addSkim}
    fi

    #-----------------------------------------------------------------------------------------------
    # Generate the catalog entries
    #-----------------------------------------------------------------------------------------------
    if [ "$generate" == 1 ] && ( [ "$newFiles" == 1 ] || [ "$catalog" != 1 ] )
    then
      generateCatalog.py --nFilesPerSet=$nFilesPerSet \
                         --rawFile=$cataDir/$mitCfg/$VERSION/$dataset${addSkim}
    fi

  fi
done

exit 0
