#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Download exactly one file, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Nov 18, 2010)
#---------------------------------------------------------------------------------------------------
echo " ";echo " ==== JOB ENVIRONMENT ==== ";echo " "; whoami;id;/bin/hostname;pwd
echo " ";echo " ==== START JOB WITH ARGUMENTS: $* ====";echo " "

klist
cp /home/cmsprod/.krb5/x509up_u5410 /tmp/
cp /home/cmsprod/.krb5/ticket       /tmp/krb5cc_5410
ls -lhrt /tmp/krb5cc_5410
export KRB5CCNAME=FILE:/tmp/krb5cc_5410
klist

dataFile=$1
target=$2
procId=$$
logFile=`echo $dataDir/$dataFile | tr '/' '+'`
logFile=/tmp/$logFile

echo " DataFile: $dataFile  to: $target"

echo " "; echo "Initialize CMSSW"; echo " "
pwd
pwd=`pwd`

export SCRAM_ARCH=slc5_ia32_gcc434
export VO_CMS_SW_DIR=~cmsprod/cmssoft
source $VO_CMS_SW_DIR/cmsset_default.sh
cd     $HOME/cms/cmssw/016/CMSSW_3_8_6_patch1/src
eval   `scram runtime -sh`
source $CMSSW_BASE/src/MitProd/Processing/bin/processing.sh
cd $pwd
#list $dataDir

# Get ready to run
rm -f $logFile

echo " "; echo "Starting download now"; echo " "
if   [ "`echo $dataFile | grep /castor/cern.ch`" != "" ]
then
  storageEle='srm-cms.cern.ch'
  storagePath='/srm/managerv2?SFN='
  storageUrl="srm://${storageEle}:8443${storagePath}$dataFile"
  echo " "; echo " Staging all file into castor ...."; echo " "
  echo "lcg-cp $storageUrl $target"
  lcg-cp $storageUrl $target
elif [ "`echo $dataFile | grep /pnfs/cmsaf.mit.edu`" != "" ]
then
  #storageEle="se01.cmsaf.mit.edu"
  storageEle="t2srv0012.cmsaf.mit.edu"
  storagePath=''
  storageUrl="dcap://${storageEle}/$dataFile"
  echo "dccp $storageUrl $target"
  dccp $storageUrl $target
else
  echo ' Copy mechanism not known.'
  exit 1
fi


rm -f $logFile

exit 0
