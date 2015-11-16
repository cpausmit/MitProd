#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Rename an existing sample (so far all potentially exisiting catalog info will have to be redone)
# Careful to make sure CERN is working you need to activate the ssh tunnel:
#
#   sshCernTunnel dtmit08
#
#---------------------------------------------------------------------------------------------------
CERN_ACTIVE=0
CATALOG=~cmsprod/catalog
## testTunnel=`ps auxw | grep -v grep | grep sshCernTunnel`
## if [ "$testTunnel" == "" ]
## then
##   echo " ERRO - it seems the cern ssh tunnel is not active"
##   echo "        to start it use: sshCernTunnel dtmit08"
##   echo ""
##   exit 1
## fi

domain=`echo $HOSTNAME | cut -d'.' -f2-100`
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

## # For CERN
## CERN_SERVER="srm://srm-cms.cern.ch:8443/srm/managerv2?SFN="
## CERN_LOCATION="/castor/cern.ch/user/p/paus"
## CERN_LOCAL_LOCATION="/data/hadoop/cmsprod"
## 
## klist -s
## if [ $? != 0 ]; then
##   kinit -f
## fi

BOOK="$1"
SOURCE="$2"
TARGET="$3"

echo ""
echo "Trying to rename sample: $SOURCE to $TARGET"
## echo "                at CERN: ${CERN_SERVER}${CERN_LOCATION}/$BOOK"
echo "                at  MIT: ${SERVER}${LOCATION}/$BOOK"
echo ""
echo ""

## # Start with CERN
## 
## if [ "$CERN_ACTIVE" != "0" ]
## then
## 
##   sourceExists=`srmls -recursion_depth=0 ${CERN_SERVER}${CERN_LOCATION}/${BOOK}/$SOURCE 2> /dev/null`
##   echo " Existing - source (CERN): $sourceExists"
##   targetExists=`srmls -recursion_depth=0 ${CERN_SERVER}${CERN_LOCATION}/${BOOK}/$TARGET 2> /dev/null`
##   echo " Existing - target (CERN): $targetExists"
##   
##   # first the original files
##   if [ ".$sourceExists" != "." ] && [ ".$targetExists" == "." ]
##   then
##     echo " Move data files at CERN"
##     move ${CERN_LOCATION}/${BOOK}/$SOURCE  ${CERN_LOCATION}/${BOOK}/$TARGET
##     # next are the cern catalogs
##     existSource=`ssh localhost -p 2222 ls -1 catalog/cern/${BOOK}/$SOURCE 2> /dev/null`
##     if [ "$existSource" != "" ]
##     then
##       echo " Moving catalogs at CERN"
##       ssh localhost -p 2222 \
##         "cd catalog/cern/${BOOK}; mv $SOURCE $TARGET; cd $TARGET; repstr $SOURCE $TARGET Filesets RawFiles.??"
##     else
##       echo " No catalogs found at CERN"
##     fi
##   else
##     echo " No data files found at CERN"
##   fi
##   
##   # now the local CERN files
##   existSource=`ssh localhost -p 2222 ls -1 ${CERN_LOCAL_LOCATION}/${BOOK}/$SOURCE 2> /dev/null`
##   existTarget=`ssh localhost -p 2222 ls -1 ${CERN_LOCAL_LOCATION}/${BOOK}/$TARGET 2> /dev/null`
##   if [ "$existSource" != "" ] && [ "$existTarget" == "" ]
##   then
##     echo " Moving local samples: mv ${CERN_LOCAL_LOCATION}/${BOOK}/$SOURCE ${CERN_LOCAL_LOCATION}/${BOOK}/$TARGET"
##     ssh localhost -p 2222 mv ${CERN_LOCAL_LOCATION}/${BOOK}/$SOURCE ${CERN_LOCAL_LOCATION}/${BOOK}/$TARGET
##     # next are the cern local catalogs
##     existSource=`ssh localhost -p 2222 ls -1 catalog/local/${BOOK}/$SOURCE 2> /dev/null`
##     if [ "$existSource" != "" ]
##     then
##       echo " Moving local catalogs at CERN"
##       ssh localhost -p 2222 \
##         "cd catalog/local/${BOOK}; mv $SOURCE $TARGET; cd $TARGET; repstr $SOURCE $TARGET Filesets RawFiles.??"
##     else
##       echo " No local catalogs found at CERN"
##     fi
##   else
##     echo " No local data files found at CERN"
##   fi
## fi

# Continue with MIT

sourceExists=`srmls -recursion_depth=0 ${SERVER}${LOCATION}/${BOOK}/$SOURCE 2> /dev/null`
echo " Existing - source  (MIT): $sourceExists"
targetExists=`srmls -recursion_depth=0 ${SERVER}${LOCATION}/${BOOK}/$TARGET 2> /dev/null`
echo " Existing - target  (MIT): $targetExists"

# first the original files
if [ ".$sourceExists" != "." ] && [ ".$targetExists" == "." ]
then
  echo " Moving data files at MIT"
  move ${LOCATION}/${BOOK}/$SOURCE       ${LOCATION}/${BOOK}/$TARGET
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
  echo " Moving local samples at MIT: move ${LOCAL_LOCATION}/${BOOK}/$SOURCE ${LOCAL_LOCATION}/${BOOK}/$TARGET"
  move ${LOCAL_LOCATION}/${BOOK}/$SOURCE ${LOCAL_LOCATION}/${BOOK}/$TARGET
  cd     $CATALOG/local/$BOOK
  if [ -d $SOURCE ]
  then
    echo " Moving local catalogs at MIT"
    mv     $SOURCE $TARGET
    cd     $TARGET
    repstr $SOURCE $TARGET  Filesets RawFiles.??
  else
    echo " No local catalogs found at MIT"
  fi
else
  echo " No local data files found at MIT"
fi

exit 0
