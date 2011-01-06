#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Download exactly one file, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Nov 18, 2010)
#---------------------------------------------------------------------------------------------------
echo " ";echo " ==== JOB ENVIRONMENT ==== ";echo " "; whoami;id;/bin/hostname;pwd
echo " ";echo " ==== START JOB WITH ARGUMENTS: $* ====";echo " "

klist
id=`id -u`
cp ~/.krb5/x509up_u${id} /tmp/
cp ~/.krb5/ticket        /tmp/krb5cc_${id}
ls -lhrt /tmp/krb5cc_${id}
export KRB5CCNAME=FILE:/tmp/krb5cc_${id}
klist

dataFile=$1
target=$2
procId=$$

echo " DataFile: $dataFile  to: $target"

echo " "; echo "Initialize CMSSW"; echo " "
pwd
pwd=`pwd`

export SCRAM_ARCH=slc5_ia32_gcc434
export VO_CMS_SW_DIR=/osg/app/cmssoft/cms
source $VO_CMS_SW_DIR/cmsset_default.sh
cd     $HOME/cms/cmssw/017/CMSSW_3_9_5_patch1/src
eval   `scram runtime -sh`
source $CMSSW_BASE/src/MitProd/Processing/bin/processing.sh
cd $pwd

# make storage Urls for target and source

targetUrl=$target
if   [ "`echo $target | grep /pnfs/cmsaf.mit.edu`" != "" ]
then
  storageEle="se01.cmsaf.mit.edu"
  storagePath='/srm/managerv2?SFN='
  targetUrl="srm://${storageEle}:8443${storagePath}$target"
elif [ "`echo $target | grep /castor/cern.ch`" != "" ]
then
  storageEle='srm-cms.cern.ch'
  storagePath='/srm/managerv2?SFN='
  targetUrl="srm://${storageEle}:8443${storagePath}$target"
fi

sourceUrl=$dataFile
if   [ "`echo $dataFile | grep /pnfs/cmsaf.mit.edu`" != "" ]
then
  storageEle="se01.cmsaf.mit.edu"
  storagePath='/srm/managerv2?SFN='
  sourceUrl="srm://${storageEle}:8443${storagePath}$dataFile"
elif [ "`echo $dataFile | grep /castor/cern.ch`" != "" ]
then
  storageEle='srm-cms.cern.ch'
  storagePath='/srm/managerv2?SFN='
  sourceUrl="srm://${storageEle}:8443${storagePath}$dataFile"
fi

echo " "; echo "Starting download now"; echo " "
echo "srm-copy $sourceUrl $targetUrl"
srm-copy $sourceUrl $targetUrl

exit 0
