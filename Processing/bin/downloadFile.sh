#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Download exactly one file, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Nov 18, 2010)
#---------------------------------------------------------------------------------------------------
echo " ";echo " ==== JOB ENVIRONMENT ==== ";echo " "; whoami;id;/bin/hostname;pwd
echo " ";echo " ==== START JOB WITH ARGUMENTS: $* ====";echo " "

LCGCP='lcg-cp'
SRMCP='srmcp'

dataFile=$1
target=$2
procId=$$

echo " DataFile: $dataFile  to: $target"

echo " "; echo "Initialize CMSSW"; echo " "
pwd
pwd=`pwd`

# show the certificate
voms-proxy-info -all

# legacy but works on 32 bit machines
if [ "`uname -p`" != "x86_64" ]
then
  source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env_3_1.sh
fi

#export SCRAM_ARCH='slc5_ia32_gcc434'
#export VO_CMS_SW_DIR=/server/01a/mitdata/cmssoft
#source $VO_CMS_SW_DIR/cmsset_default.sh
#cd     $HOME/cms/cmssw/019/CMSSW_3_9_7/src
#eval   `scram runtime -sh`
#source $CMSSW_BASE/src/MitProd/Processing/bin/processing.sh
#cd $pwd

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

#echo "$SRMCP -2 $sourceUrl $targetUrl"
#$SRMCP -2 $sourceUrl $targetUrl

exit 0
