#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Upload exactly one file to dropbox, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Sep 30, 2015)
#---------------------------------------------------------------------------------------------------
# some basic printing
h=`basename $0`
echo " ${h}: Show who and where we are!"
echo " Script:    $h"
echo " Arguments: ($*)"
echo " "
echo " start time    : "`date`
echo " user executing: "`whoami`" --> "`id`
echo " running on    : "`/bin/hostname`
echo " executing in  : "`pwd`
echo " ";

XRDROOT='root://xrootd.cmsaf.mit.edu/'
XRDCP='xrdcp'
DRBOX='/home/cmsprod/Tools/Dropbox-Uploader/dropbox_uploader.sh'

fullFile=$1
file=`  basename $fullFile`
subdir=`dirname  $fullFile`
dir=`   dirname  $subdir`

echo " DataFile: $dataFile  to: $target"

echo " "; echo "Initialize CMSSW"; echo " "
pwd
pwd=`pwd`

# take care of the certificate
if [ -e "./x509up_u`id -u`" ]
then
  export X509_USER_PROXY="./x509up_u`id -u`"
elif [ -e "/tmp/x509up_u`id -u`" ]
then
  export X509_USER_PROXY="/tmp/x509up_u`id -u`"
else
  echo " ERROR - no ticket file, transfer will fail!"
fi
echo " INFO -- using the x509 ticket: $X509_USER_PROXY"

# show the certificate
voms-proxy-info -all

echo " "; echo "Starting upload operation now"; echo " "

# download locally first
echo " -- download locally"
#echo "$XRDCP $XRDROOT$fullFile ./$file"
#$XRDCP $XRDROOT$fullFile ./$file

echo "lcg-cp -D srmv2 -b \
   srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN=/mnt/hadoop/cms${fullFile} \
   file://`pwd`/$file"
lcg-cp -D srmv2 -b \
   srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN=/mnt/hadoop/cms${fullFile} \
   file://`pwd`/$file
rc=$?
if [ "$rc" == "0" ]
then
  echo " local download worked."
else
  echo " local download failed (rc=$rc)."
  exit 0
fi


# prepare the upload
echo " -- prepare upload"
echo "$DRBOX mkdir  /cms$dir"
$DRBOX mkdir  /cms$dir
echo "$DRBOX mkdir  /cms$subdir"
$DRBOX mkdir  /cms$subdir

# upload
echo " -- upload"
echo "$DRBOX upload ./$file /cms$fullFile"
      $DRBOX upload ./$file /cms$fullFile

rc=$?
if [ "$rc" == "0" ]
then
  echo " upload worked. cleaning up."
else
  echo " upload failed (rc=$rc). cleaning up."
fi

# make sure to cleanup
echo " -- cleanup"
echo "rm -f ./$file"
rm -f ./$file

exit 0
