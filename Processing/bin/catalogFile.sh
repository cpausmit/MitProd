#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Catalog exactly one file, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Dec 09, 2008)
#---------------------------------------------------------------------------------------------------
echo " ";echo " ==== JOB ENVIRONMENT ==== ";echo " "; whoami;id;/bin/hostname;pwd
echo " ";echo " ==== START JOB WITH ARGUMENTS: $* ====";echo " "

dataDir=$1
dataFile=$2
procId=$$
logFile=`echo $dataDir/$dataFile | tr '/' '+'`
logFile=/tmp/$logFile

echo " DataFile: $dataFile  in DataDir: $dataDir"
if [ "`echo $dataDir | grep castor`" != "" ]
then
  echo " "; echo " Staging all file into castor ...."; echo " "
  $HOME/bin/stageSample.py --dataDir=$dataDir --pattern=$dataFile
fi

echo " "; echo "Initialize CMSSW"; echo " "
pwd
pwd=`pwd`

export SCRAM_ARCH=slc5_ia32_gcc434
export VO_CMS_SW_DIR=/server/02a/cmsprod/cmssoft
source $VO_CMS_SW_DIR/cmsset_default.sh
cd     $HOME/cms/cmssw/014/CMSSW_3_7_0_patch2/src
eval   `scram runtime -sh`
source $CMSSW_BASE/src/MitProd/Processing/bin/processing.sh
cd $pwd
#list $dataDir

# Get ready to run
rm -f $logFile

echo " "; echo "Starting root now"; echo " "
which root
ls -lhrt $CMSSW_BASE/src/MitAna/macros/

echo " root -l -b -q $CMSSW_BASE/src/MitProd/Processing/root/rootlogon.C \ "
echo "               $CMSSW_BASE/src/MitProd/Processing/root/runFileCataloger.C \ "
echo "               ($dataDir,$dataFile) \ "
echo "               >& $logFile "

root -l -b -q \
     $CMSSW_BASE/src/MitProd/Processing/root/rootlogon.C \
     $CMSSW_BASE/src/MitProd/Processing/root/runFileCataloger.C\(\"$dataDir\",\"$dataFile\"\) \
     >& $logFile
  
status=`echo $?`
error=`cat $logFile |grep -v mithep::Selector::UpdateRunInfo | grep Error | wc -l`
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
