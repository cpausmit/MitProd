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
mitCfg=filefi
while getopts "cegtm:n:" o
do case "$o" in
  c) catalog=1;;
  e) extract=1;;
  g) generate=1;;
  t) test=1;;
  m) mitCfg=$OPTARG;;
  n) nFilesPerSet=$OPTARG;;
  [?]) echo "usage: $H [-cegt] [-m <mitcfg> ] [-n <nEvts> ] <version> [ <pattern> [ <location> ] ]"
       exit 1;;
  esac
done
let "shiftIdx = $OPTIND - 1"
shift $shiftIdx

#MIT_LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
MIT_LOCATION="/mnt/hadoop/cms/store/user/paus"
CERN_LOCATION="/castor/cern.ch/user/p/paus"
CATALOG=$HOME/catalog
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
   [ "`echo $LOCATION | grep /pnfs/cmsaf.mit.edu/`" == "" ] && \
   [ "`echo $LOCATION | grep /mnt/hadoop/cms/store/`" == "" ] && \
   [ "$LOCATION" != "" ]
then
  if [ "`echo $LOCATION | grep /cmsprod/skim/`" == "" ]
  then
    CATALOG=$CATALOG/local
  else
    CATALOG=$CATALOG/local/` basename \`dirname $LOCATION\` `/`basename $LOCATION`
  fi
elif [ ".`echo $HOSTNAME | grep cern.ch`" != "." ]
then
  LOCATION=$CERN_LOCATION
  CATALOG=$CATALOG/cern
elif [ ".`echo $HOSTNAME | grep mit.edu`" != "." ]
then
  LOCATION=$MIT_LOCATION
  CATALOG=$CATALOG/t2mit
fi

# Create a list of the datsets we need to catalog
LIST=`list ${LOCATION}/$mitCfg/$VERSION | cut -d ' ' -f2`
if [ "`echo $PATTERN | grep crab_0_`" != ""  ]
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
#   $CATALOG (location of the catalog)
#   in book:                         $mitCfg/$VERSION
#   in dataset matching the pattern: $PATTERN
#
#
# /mnt/hadoop/cmsprod/skim/skim-dilepton-v1/t2mit/
#   filefi/014/p10-wjets-mg-v26/skim-dilepton-v1_p10-wjets-mg-v26_noskim_00
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

    #echo " "
    echo " CATALOG - Lfns: $nCata (done)  $nProc (to catalog) / $nTota (total: $mitCfg/$VERSION/${dataset}.lfns_$crabId)"
    echo " "

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
    extractCatalog.py --compact --catalog=$CATALOG --mitCfg=$mitCfg --version=$VERSION \
                      --dataset=$extDataset
   
    #-----------------------------------------------------------------------------------------------
    # Submit the cataloging jobs
    #-----------------------------------------------------------------------------------------------
    if   [ "$catalog" == 1 ]
    then
      catalogFiles.sh   $LOCATION $CATALOG $mitCfg/$VERSION $extDataset
      echo ""
    fi

    # start waiting time now
    startTime=$(date +%s)

    # Figure out whether there are new files to consider
    newFiles=0
    if [ "`ls -1 $CATALOG/condor/$mitCfg/$VERSION/$extDataset | grep root`" != "" ]
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
      jobs=`condor_q -global cmsprod -l | egrep '^Args.*root' | grep $extDataset | wc -l`
      while [ "$jobs" != "0" ]
      do
	echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"
	sleep 10
	echo ""
	jobs=`condor_q -global cmsprod -l | egrep '^Args.*root' | grep $extDataset | wc -l`
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
                        --catalog=$CATALOG --mitCfg=$mitCfg --version=$VERSION --dataset=$extDataset
    fi

    #-----------------------------------------------------------------------------------------------
    # Generate the catalog entries
    #-----------------------------------------------------------------------------------------------
    if [ "$generate" == 1 ] && ( [ "$newFiles" == 1 ] || [ "$catalog" != 1 ] )
    then
      generateCatalog.py --nFilesPerSet=$nFilesPerSet --rawFile=$CATALOG/$mitCfg/$VERSION/$dataset
    fi

  fi
done

exit 0
