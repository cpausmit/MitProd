#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Remove an existing sample from disk and from catalog
#
#### CAREFUL WILL NOT WORK FOR CERN ####
#
#---------------------------------------------------------------------------------------------------
domain=`echo $HOSTNAME | cut -d'.' -f2-100`
CATALOG=~cmsprod/catalog

if [ "$domain" == "mit.edu" ]
then
  # For MIT
  SERVER="srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN="
  LOCATION="/mnt/hadoop/cms/store/user/paus"
  LOCAL_LOCATION="/mnt/hadoop/cmsprod"
else
  # For CERN
  SERVER="srm://srm-cms.cern.ch:8443/srm/managerv2?SFN="
  LOCATION="/castor/cern.ch/user/p/paus"
  LOCAL_LOCATION="/data/hadoop/cmsprod"
fi

klist -s
if [ $? != 0 ]; then
  kinit -f
fi

BOOK="$1"
SAMPLE="$2"
LOCATION="$3"

echo ""
echo "Trying to remove sample: $SAMPLE ($BOOK) from $LOCATION"

#### CAREFUL WILL NOT WORK FOR CERN ####

if [ "$LOCATION" == "" ] || [ "$LOCATION" == "T2MIT" ]
then
  # remove the remote physical files
  glexec ./removeSample.sh ${BOOK} $SAMPLE exec
  # remove remote catalogs
  if [ -d $CATALOG/t2mit/$BOOK/$SAMPLE ]
  then
    rm -rf $CATALOG/t2mit/$BOOK/$SAMPLE
  fi
fi

if [ "$LOCATION" == "" ] || [ "$LOCATION" == "LOCAL" ]
then
  # remove the local files
  if [ -d "${LOCAL_LOCATION}/${BOOK}/$SAMPLE" ]
  then
    rm -rf ${LOCAL_LOCATION}/${BOOK}/$SAMPLE
  fi
  # remove local catalogs
  if [ -d $CATALOG/local/$BOOK/$SAMPLE ]
  then
    rm -rf $CATALOG/local/$BOOK/$SAMPLE
  fi
fi

exit 0
