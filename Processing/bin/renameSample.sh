#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Rename an existing sample (so far all potentially exisiting catalog info will have to be redone)
#---------------------------------------------------------------------------------------------------
SERVER="srm://se01.cmsaf.mit.edu:8443/srm/managerv2?SFN="
LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus/filefi/014"
#SERVER="srm://srm-cms.cern.ch:8443/srm/managerv2?SFN="
#LOCATION="/castor/cern.ch/user/p/paus/filefi/014"

SOURCE="$1"
TARGET="$2"

sourceExists=`srmls ${SERVER}${LOCATION} | grep $SOURCE`
echo " Existing - source: $sourceExists"
targetExists=`srmls ${SERVER}${LOCATION} | grep $TARGET`
echo " Existing - target: $targetExists"

if [ ".$sourceExists" != "." ] && [ ".$targetExists" == "." ]
then

  #echo \
  move ${LOCATION}/$SOURCE \
       ${LOCATION}/$TARGET

  ##for file in `list ${SERVER}${LOCATION}/$TARGET | grep '   ' | tr -s ' ' | cut -d ' ' -f 3`
  ##do
  ##
  ##  file=`basename $file | cut -d '_' -f2-3`
  ##  echo \
  ##    move ${SERVER}${LOCATION}/$TARGET/${SOURCE}_$file \
  ##         ${SERVER}${LOCATION}/$TARGET/${TARGET}_$file
  ##
  ##done
fi

cd     ~/catalog/t2mit/filefi/014
if [ -d $SOURCE ]
then
  mv     $SOURCE $TARGET
  cd     $TARGET
  ~paus/bin/repstr $SOURCE $TARGET Filesets RawFiles.??
fi

cd     ~/catalog/local/filefi/014
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
