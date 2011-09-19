#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Rename an existing sample (so far all potentially exisiting catalog info will have to be redone)
#---------------------------------------------------------------------------------------------------
SERVER="srm://se01.cmsaf.mit.edu:8443/srm/managerv2?SFN="
LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
BOOK="filefi/${MIT_VERS}"
LOCAL_LOCATION="/mnt/hadoop/cmsprod"
CATALOG="/home/cmsprod/catalog"
#SERVER="srm://srm-cms.cern.ch:8443/srm/managerv2?SFN="
#LOCATION="/castor/cern.ch/user/p/paus/filefi/014"
klist -s
if [ $? != 0 ]; then
  kinit -f
fi

SOURCE="$1"
TARGET="$2"

echo ""
echo "Trying to rename sample: $SOURCE to $TARGET"
echo "                     in: $SERVER/$LOCATION/$BOOK"
echo ""
echo ""

sourceExists=`srmls -recursion_depth=0 ${SERVER}${LOCATION}/${BOOK}/$SOURCE 2> /dev/null`
echo " Existing - source: $sourceExists"
targetExists=`srmls -recursion_depth=0 ${SERVER}${LOCATION}/${BOOK}/$TARGET 2> /dev/null`
echo " Existing - target: $targetExists"

# first the original files
if [ ".$sourceExists" != "." ] && [ ".$targetExists" == "." ]
then
  move ${LOCATION}/${BOOK}/$SOURCE       ${LOCATION}/${BOOK}/$TARGET
fi

# now the local files
if [ -d "${LOCAL_LOCATION}/${BOOK}/$SOURCE" ] && ! [ -d "${LOCAL_LOCATION}/${BOOK}/$TARGET" ]
then
  move ${LOCAL_LOCATION}/${BOOK}/$SOURCE ${LOCAL_LOCATION}/${BOOK}/$TARGET
fi

# next are the catalogs
cd     $CATALOG/t2mit/$BOOK
if [ -d $SOURCE ]
then
  mv     $SOURCE $TARGET
  cd     $TARGET
  ~paus/bin/repstr $SOURCE $TARGET Filesets RawFiles.??
fi

cd     $CATALOG/local/$BOOK
if [ -d $SOURCE ]
then
  mv     $SOURCE $TARGET
  cd     $TARGET
  ~paus/bin/repstr $SOURCE $TARGET  Filesets RawFiles.??
fi


exit 0

## MIT_LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
## CERN_LOCATION="/castor/cern.ch/user/p/paus"
## CATALOG=$HOME/catalog
## 
## # Health checks
## if [ ".$1" == "." ]
## then
##   H=`basename $0`
##   echo "";echo " usage: $H  <version>  [ <pattern> [ <location> ] ]"; echo ""
##   exit 1
## fi
## 
## # Decode command line parameters
## VERSION="$1"
## PATTERN="empty"
## if [ ".$2" != "." ]
## then
##   PATTERN=$2
## fi
## LOCATION=$MIT_LOCATION
## if [ ".$3" != "." ]
## then
##   LOCATION=$3
## fi
## 
## # Create a list of the datsets we need to catalog
## echo ""
## if   [ "`echo $LOCATION | grep castor`" != "" ]
## then
##   echo " Identified a castor sample request. Using rfdir on $LOCATION"
##   CATALOG=$CATALOG/cern
##   LIST=`rfdir $LOCATION/filler/$VERSION | tr -s ' ' | cut -d ' ' -f 9`
## elif [ "`echo $LOCATION | grep cmsaf.mit.edu`" != "" ]
## then
##   echo " Identified a tier-2 sample request. Using srmls on $LOCATION"
##   CATALOG=$CATALOG/t2mit
##   LIST=`srmls ${SERVER}${LOCATION}/filler/$VERSION | grep '   ' | tr -s ' ' | cut -d ' ' -f 3`
## else
##   echo " Identified a local sample request. Using ls on $LOCATION"
##   CATALOG=$CATALOG/local
##   LIST=`ls -1 $LOCATION/filler/$VERSION`
## fi
## 
## # Show me the list
## echo ""
## echo "== LIST =="
## echo $LIST
## 
## # Loop through the list and catalog
## echo ""
## for dataset in $LIST; do
##   dataset=`basename $dataset`
##   if [ "$PATTERN" == "empty" ] || [ "`echo $dataset | grep $PATTERN`" != "" ]
##   then
##     ./catalogFiles.csh $LOCATION $CATALOG filler/$VERSION $dataset
##   fi
## done
