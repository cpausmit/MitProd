#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Catalog exactly one file, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Dec 09, 2008)
#---------------------------------------------------------------------------------------------------
echo " ";echo " ==== JOB ENVIRONMENT ==== ";echo " "; whoami;id;/bin/hostname;pwd
echo " ";echo " ==== START JOB WITH ARGUMENTS: $* ====";echo " "

export CATALOG_MACRO="runFileCataloger.C"

export SCRAM_ARCH='slc5_ia32_gcc434'
export  VO_CMS_SW_DIR=~cmsprod/cmssoft
source $VO_CMS_SW_DIR/cmsset_default.sh
cd     ~cmsprod/cms/cmssw/018/CMSSW_3_9_7/src
eval   `scram runtime -sh`
source $CMSSW_BASE/src/MitProd/Processing/bin/processing.sh
cd $pwd

voms-proxy-info -all

dataDir=$1
dataFile=$2
if [ ".$2" == "." ]
then
  dataFile=`basename $dataDir`
  dataDir=`dirname $dataDir`
fi

if ! [ -z "`echo $dataDir | grep delphes`" ] || ! [ -z "`echo $dataDir | grep user/paus/mc`" ]
then
  CATALOG_MACRO="runSimpleFileCataloger.C"
fi

procId=$$
logFile=`echo $dataDir/$dataFile | tr '/' '+'`
logFile=/tmp/$logFile

echo " "; echo "Initialize CMSSW"; echo " "
pwd
pwd=`pwd`

# Get ready to run
rm -f $logFile

echo " "; echo "Starting root now"; echo " "
which root
ls -lhrt $CMSSW_BASE/src/MitAna/macros/

echo " root -l -b -q $CMSSW_BASE/src/MitProd/Processing/root/rootlogon.C \ "
echo "               $CMSSW_BASE/src/MitProd/Processing/root/${CATALOG_MACRO} \ "
echo "               ($dataDir,$dataFile) \ "
echo "               >& $logFile "

root -l -b -q \
     $CMSSW_BASE/src/MitProd/Processing/root/rootlogon.C \
     $CMSSW_BASE/src/MitProd/Processing/root/${CATALOG_MACRO}\(\"$dataDir\",\"$dataFile\"\) \
     >& $logFile
  
status=`echo $?`
error=`cat $logFile |grep -v mithep::Selector::UpdateRunInfo | grep -v 'no dictionary for class' | grep 'Error' | wc -l`
zip=`grep R__unzip: $logFile | wc -l`
  
echo " "
echo "Status: $status  Errors: $error  R__Unzip: $zip"
if [ $status == 0 ] && [ $error == 0 ] && [ $zip == 0 ]
then
  cat $logFile
  echo "DECISION"
  echo "  File  $dataDir/$dataFile  looks healthy, make entry into cataloging database."
  echo " "
  echo -n " XX-CATALOG-XX "
  tail -1 $logFile 
else
  echo " ==== DUMPING LOGFILE NOW ===="
  cat $logFile
  echo "DECISION"
  echo "  File  $dataDir/$dataFile  looks corrupted, remove it."
fi

rm -f $logFile

exit 0
