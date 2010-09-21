#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Perform a number of cataloging action on a bunch of datasets
#---------------------------------------------------------------------------------------------------
H=`basename $0`
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

#echo " >>  C: $catalog  E: $extract  G: $generate  T: $test  M: $mitCfg"
#echo " >> $shiftIdx <<  Rest: $@"
#exit 0

MIT_LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
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
echo ""
if [ "`echo $LOCATION | grep /castor/cern.ch/`" == "" ] && \
   [ "`echo $LOCATION | grep /pnfs/cmsaf.mit.edu/`" == "" ] && \
   [ "$LOCATION" != "" ]
then
  CATALOG=$CATALOG/local
  echo " Identified a local sample request. Using ls on $LOCATION"
elif [ ".`echo $HOSTNAME | grep cern.ch`" != "." ]
then
  LOCATION=$CERN_LOCATION
  CATALOG=$CATALOG/cern
  echo " Identified a castor sample request ($LOCATION,$CATALOG)"
elif [ ".`echo $HOSTNAME | grep mit.edu`" != "." ]
then
  LOCATION=$MIT_LOCATION
  CATALOG=$CATALOG/t2mit
  echo " Identified a tier-2 sample request ($LOCATION,$CATALOG)"
fi

# Create a list of the datsets we need to catalog
echo ""
LIST=`list ${LOCATION}/$mitCfg/$VERSION | cut -d ' ' -f2`
if [ "`echo $PATTERN | grep crab_0_`" != ""  ]
then
  echo " Official dataset with a crab subdir: $PATTERN"
  LIST="${LOCATION}/$mitCfg/$VERSION/$PATTERN"
fi

#---------------------------------------------------------------------------------------------------
# Loop through the list and catalog --> ACTION STARTS BELOW HERE
#---------------------------------------------------------------------------------------------------
startTime=$(date +%s)
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

    echo " Cataloging status: $nCata (done)  $nProc (to catalog) / $nTota (total: $lfnFile)"
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

    # Submit the cataloging jobs
    if   [ "$catalog" == 1 ]
    then
      #echo \
      catalogFiles.csh   $LOCATION $CATALOG $mitCfg/$VERSION $extDataset
      echo ""
    fi

    # Figure out whether there are new files to consider
    newFiles=0
    if [ "`ls -1 $CATALOG/condor/$mitCfg/$VERSION/$extDataset | grep root`" != "" ]
    then
      newFiles=1
    fi
    #echo " New files: $newFiles ($extract, $catalog)"

    # Extract the catalog metadata
    if [ "$extract" == 1 ] && ( [ "$newFiles" == 1 ] || [ "$catalog" != 1 ] )
    then

      #echo " Extracting new files: $newFiles ($extract, $catalog)"
      #condor_q
      nowTime=$(date +%s)
      duration=$(($nowTime - $startTime))
      ## echo " -> condor_q -global cmsprod -l | grep ^Args | grep $extDataset | wc -l"
      jobs=`condor_q -global cmsprod -l | grep ^Args | grep $extDataset | wc -l`
      while [ "$jobs" != "0" ]
      do
	echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"
	sleep 10
	echo ""
	jobs=`condor_q -global cmsprod -l | grep ^Args | grep $extDataset | wc -l`
        nowTime=$(date +%s)
        duration=$(($nowTime - $startTime))
      done
      echo " Queues are empty ($jobs) -->  moving on and extract cataloging results."
      echo ""

      #echo \
      extractCatalog.py $OPTION \
                        --catalog=$CATALOG --mitCfg=$mitCfg --version=$VERSION --dataset=$extDataset
    fi

    # Generate the catalog entries
    if [ "$generate" == 1 ] && ( [ "$newFiles" == 1 ] || [ "$catalog" != 1 ] )
    then
      #echo \
      generateCatalog.py --nFilesPerSet=$nFilesPerSet --rawFile=$CATALOG/$mitCfg/$VERSION/$dataset
    fi

  fi
done

exit 0
