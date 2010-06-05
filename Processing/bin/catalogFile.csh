#!/bin/tcsh
#---------------------------------------------------------------------------------------------------
# Catalog exactly one file, either interactively or submitting to condor batch system.
#
#                                                                             Ch.Paus (Dec 09, 2008)
#---------------------------------------------------------------------------------------------------
echo " ";echo " ==== JOB ENVIRONMENT ==== ";echo " "; whoami;id;/bin/hostname;pwd
echo " ";echo " ==== START JOB WITH ARGUMENTS: $* ====";echo " "

set dataDir=$1
set dataFile=$2
set procId=$$
set logFile=`echo $dataDir/$dataFile | tr '/' '+'`
set logFile=/tmp/$logFile

echo " DataFile: $dataFile  in DataDir: $dataDir"
if ( "`echo $dataDir | grep castor`" != "" ) then
  echo " "; echo " Staging all file into castor ...."; echo " "
  $HOME/bin/stageSample.py --dataDir=$dataDir --pattern=$dataFile
endif

echo " "; echo "Initialize CMSSW"; echo " "
pwd
set pwd=`pwd`

setenv SCRAM_ARCH    slc5_ia32_gcc434
setenv VO_CMS_SW_DIR /server/02a/cmsprod/cmssoft
source $VO_CMS_SW_DIR/cmsset_default.csh
cd     $HOME/cms/cmssw/013/CMSSW_3_5_6/src
eval   `scram runtime -csh`
source $CMSSW_BASE/src/MitProd/Processing/bin/processing.csh
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
  
set status=`echo $?`
set error=`cat $logFile |grep -v mithep::Selector::UpdateRunInfo | grep Error | wc -l`
set zip=`grep R__unzip: $logFile | wc -l`
  
echo " "
echo "Status: $status  Errors: $error  R__Unzip: $zip"
if ( $status == 0 && $error == 0 && $zip == 0 ) then
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
endif

rm -f $logFile

exit 0
