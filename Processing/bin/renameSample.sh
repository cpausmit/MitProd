#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Rename an existing sample (so far all potentially exisiting catalog info will have to be redone).
# Careful, we have to update the Tier-2 the Tier-3 cache and the corresponding catalog.
#
#---------------------------------------------------------------------------------------------------
CERN_ACTIVE=0
CATALOG=~cmsprod/catalog

SERVER="srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN="
LOCATION="/mnt/hadoop/cms/store/user/paus"
LOCAL_LOCATION="/mnt/hadoop/cms/store/user/paus"

BOOK="$1"
SOURCE="$2"
TARGET="$3"

echo ""
echo "Trying to rename sample: $SOURCE to $TARGET"
echo "                at  MIT: ${SERVER}${LOCATION}/$BOOK"
echo ""
echo ""

sourceExists=`srmls -recursion_depth=0 ${SERVER}${LOCATION}/${BOOK}/$SOURCE 2> /dev/null`
echo " Existing - source  (MIT): $sourceExists"
targetExists=`srmls -recursion_depth=0 ${SERVER}${LOCATION}/${BOOK}/$TARGET 2> /dev/null`
echo " Existing - target  (MIT): $targetExists"

# first the original files
if [ ".$sourceExists" != "." ] && [ ".$targetExists" == "." ]
then
  echo " Moving data files at MIT"
  move ${LOCATION}/${BOOK}/$SOURCE ${LOCATION}/${BOOK}/$TARGET
  # next are the catalogs
  cd     $CATALOG/t2mit/$BOOK
  if [ -d $SOURCE ]
  then
    echo " Moving catalogs at MIT"
    mv     $SOURCE $TARGET
    cd     $TARGET
    repstr $SOURCE $TARGET Filesets RawFiles.??
  else
    echo " No catalogs found at MIT"
  fi
  # correct our central sample database
  echo " Updating our central sample database"
  repstr   $SOURCE $TARGET ~cmsprod/public_html/$BOOK/Productions.*
else
  echo " No data files found at MIT"
fi

# now the local files
if [ -d "${LOCAL_LOCATION}/${BOOK}/$SOURCE" ] && ! [ -d "${LOCAL_LOCATION}/${BOOK}/$TARGET" ]
then
  echo " Moving local samples (Tier-3):"
  echo "   --> move ${LOCAL_LOCATION}/${BOOK}/$SOURCE ${LOCAL_LOCATION}/${BOOK}/$TARGET"
  move ${LOCAL_LOCATION}/${BOOK}/$SOURCE ${LOCAL_LOCATION}/${BOOK}/$TARGET
  cd     $CATALOG/local/$BOOK
  if [ -d $SOURCE ]
  then
    echo " Moving local catalogs (Tier-3)"
    mv     $SOURCE $TARGET
    cd     $TARGET
    repstr $SOURCE $TARGET  Filesets RawFiles.??
  else
    echo " No local catalogs found on Tier-3"
  fi
else
  echo " No local data files found on Tier-3"
fi

exit 0
