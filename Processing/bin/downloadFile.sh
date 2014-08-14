#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Download exactly one file, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Nov 18, 2010)
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

LCGCP='lcg-cp'
SRMCP='srmcp'

dataFile=$1
target=$2
procId=$$

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

## legacy but works on 32 bit machines
#if [ "`uname -p`" != "x86_64" ]
#then
##  source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env_3_1.sh
#fi

# make storage Urls for target and source

targetUrl="file:///$target"
if [ "`echo $target | grep /mnt/hadoop/cms/store`" != "" ]
then
  storageEle="se01.cmsaf.mit.edu"
  storagePath='/srm/v2/server?SFN='
  targetUrl="srm://${storageEle}:8443${storagePath}$target"
elif [ "`echo $target | grep /eos/cms`" != "" ]
then
  storageEle='srm-eoscms.cern.ch'
  storagePath='/srm/v2/server?SFN='
  targetUrl="srm://${storageEle}:8443${storagePath}$target"
elif [ "`echo $target | grep /castor/cern.ch`" != "" ]
then
  storageEle='srm-cms.cern.ch'
  storagePath='/srm/managerv2?SFN='
  targetUrl="srm://${storageEle}:8443${storagePath}$target"
else
  targetUrl="file:///$target"
  #targetUrl=""
fi

sourceUrl="file:///$dataFile"
if [ "`echo $dataFile | grep /mnt/hadoop/cms/store`" != "" ]
then
  storageEle="se01.cmsaf.mit.edu"
  storagePath='/srm/v2/server?SFN='
  sourceUrl="srm://${storageEle}:8443${storagePath}$dataFile"
elif [ "`echo $dataFile | grep /eos/cms`" != "" ]
then
  storageEle='srm-eoscms.cern.ch'
  storagePath='/srm/v2/server?SFN='
  sourceUrl="srm://${storageEle}:8443${storagePath}$dataFile"
elif [ "`echo $dataFile | grep /castor/cern.ch`" != "" ]
then
  storageEle='srm-cms.cern.ch'
  storagePath='/srm/managerv2?SFN='
  sourceUrl="srm://${storageEle}:8443${storagePath}$dataFile"
fi

echo " "; echo "Starting download now"; echo " "
echo "$LCGCP -D srmv2 -b $sourceUrl $targetUrl"
$LCGCP -D srmv2 -b  $sourceUrl $targetUrl

exit 0
