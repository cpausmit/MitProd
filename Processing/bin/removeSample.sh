#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Remove an existing sample from disk, catalog and the request database.
#
#---------------------------------------------------------------------------------------------------
function usage {
  echo ""
  echo " "`basename $0`" <book>  <dataset>  [ <location> = T2MIT, '' ] "
  echo ""
  echo "   book     - book of the sample in question. Example: filefi/041"
  echo "   dataset  - name of the sample in question. Example: /JetHT/Run2015B-PromptReco-v1/AOD"
  echo "   location - location (this is bogus for now)"
  echo ""
  exit 0
}

MOUNT=/mnt/hadoop
BASE_LFN=/cms/store/user/paus
CATALOG=~cmsprod/catalog
SERVER="srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN="

# OLD # RMT2="glexec hadoop dfs -rmr"
RMT2="glexec hdfs   dfs -rm -r"
RMT3="       hdfs   dfs -rm -r"


BOOK="$1"
SAMPLE="$2"
LOCATION="$3"

if [ "$BOOK" == "" ] || [ "$SAMPLE" == "" ]
then
  echo ""
  echo " Sample and book have to be defined.... "
  usage
else
  echo ""
  echo " Removing sample: $SAMPLE from book: $BOOK"
  echo ""
fi

if [ "$LOCATION" == "" ] || [ "$LOCATION" == "T2MIT" ]
then

  # remove the remote physical files
  echo " CMD: $RMT2 $BASE_LFN/${BOOK}/$SAMPLE"
  $RMT2 $BASE_LFN/${BOOK}/$SAMPLE >& /dev/null
  echo " Return code from Tier2: $?"
  echo ""

  # remove the potentially local physical files (Tier-3)
  echo " CMD: $RMT3 $BASE_LFN/${BOOK}/$SAMPLE"
  $RMT3 $BASE_LFN/${BOOK}/$SAMPLE >& /dev/null
  echo " Return code from Tier3: $?"
  echo ""

  # remove catalogs
  if [ -d $CATALOG/t2mit/$BOOK/$SAMPLE ]
  then
    rm -rf $CATALOG/t2mit/$BOOK/$SAMPLE
  fi
fi

exit 0
