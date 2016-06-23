#!/bin/bash
#===================================================================================================
#
# Execute one job on the grid or interactively.
#
#===================================================================================================

#----------------------------------------------------------------------------------------------------
#  U S E F U L   F U N C T I O N S
#----------------------------------------------------------------------------------------------------

function exeCmd {
  # provide a small frame for each command, also allows further steering
  echo " Executing: $*"
  $*
}  

function executeCmd {
  # provide a nice frame for each command, also allows further steering

  echo " "
  echo " =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
  exeCmd $*
  echo " Completed: $*"
  echo " =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
  echo " "
  
}  

function configureSite {
  # in case we are not at a CMS site we need to have a configuration
  #
  # -- ATTENTION -- CMSSW has to be setup before calling configure site
  #

  link="/cvmfs/cms.cern.ch/SITECONF/local"
  
  echo "---- Configure Site ----"
  echo " Link       = $link"
  echo " CMSSW_BASE = $CMSSW_BASE"
  ls -lh $CMSSW_BASE/src/MitProd/Processing/tgz/siteconf.tgz

  if [ -d "`readlink $link`" ]
  then
    echo " Link exists. No action needed. ($link)"
  else
    echo " WARNING -- Link points nowhere! ($link)"
    echo "  -- unpacking private local config to recover"
    executeCmd tar fzx $CMSSW_BASE/src/MitProd/Processing/tgz/siteconf.tgz
    cd SITECONF
    rm -f local
    ln -s ./T3_US_OSG ./local
    ls -lhrt
    cd -
    # make sure this is the config to be used
    export CMS_PATH=`pwd`
  fi
}

function downloadFile {
  # download a given lfn using xrootd

  # read command line parameters
  GPACK="$1"
  LFN="$2"

  serverList="cmsxrootd.fnal.gov cms-xrd-global.cern.ch xrootd.unl.edu"

  echo ""
  echo " Make local copy of the root file with LFN: $LFN"

  for server in $serverList
  do
    echo " Trying server: $server"  

    executeCmd xrdcp -d 1 -s root://$server/$LFN ./$GPACK.root

    if [ -e "./$GPACK.root" ]
    then
      echo " Looks like copy worked on server: $server."
      break
    else
      echo " ERROR -- input file ./$GPACK.root does not exist. Failed on server: $server"
    fi
  done

  if [ -e "./$GPACK.root" ]
  then
    ls -lhrt ./$GPACK.root
  else
    echo " ERROR -- input file ./$GPACK.root does not exist. Failed on all servers: $serverList"
    echo "          EXIT now because there is no AOD* file to process."
    return
  fi
}  

function setupCmssw {
  # setup a specific CMSSW release and add the local python path

  THIS_CMSSW_VERSION="$1"
  PWD=`pwd`
  echo ""
  echo "============================================================"
  echo " Initialize CMSSW $THIS_CMSSW_VERSION"
  source /cvmfs/cms.cern.ch/cmsset_default.sh
  if [ "`echo $THIS_CMSSW_VERSION | grep ^8_`" != "" ]
  then
    export SCRAM_ARCH=slc6_amd64_gcc530
  fi
  scram project CMSSW CMSSW_$THIS_CMSSW_VERSION
  pwd
  ls -lhrt
  ls -lhrt *
  cd CMSSW_$THIS_CMSSW_VERSION/src 
  eval `scram runtime -sh`
  if [ -e  "$BASEDIR/bambu_$THIS_CMSSW_VERSION.tgz" ]
  then
    cd ..
    tar fzx $BASEDIR/bambu_$THIS_CMSSW_VERSION.tgz
    source ./src/MitProd/Processing/bin/processing.sh
  fi
  cd $PWD
  echo "============================================================"
  configureSite
  echo ""
}

function showDiskSpace {
  # implement a simple minded summary of the available disk space and usage

  [ -z $BASEDIR ] && $BASEDIR="./"

  echo ""
  echo " Disk space overview "
  echo " =================== "
  df -h $BASEDIR
  echo ""
  echo " Disk space usage "
  echo " ================ "
  du -sh $BASEDIR/*
}

function testBatch {
  # implement simple minded/not perfect test to see whether script is run in batch

  batch=0
  if [ "`echo $PWD | grep $USER`" == "" ]
  then
    batch=1
  fi
  return $batch
}

#----------------------------------------------------------------------------------------------------
#  M A I N   S T A R T S   H E R E
#----------------------------------------------------------------------------------------------------

# make sure we are locked and loaded
export BASEDIR=`pwd`
echo " Executing: $0 $* "

# command line arguments
CONFIG="$1"
VERSION="$2"
PY="$3"
TASK="$4"
GPACK="$5"
LFN="$6"
CRAB="$7"

# load all parameters relevant to this task
echo " Initialize package"
cmsswVersion=`echo bambu_*.tgz | sed -e 's@bambu_@@' -e 's@.tgz@@'`

# make sure to contain file mess
mkdir ./work
cd    ./work
export WORKDIR=`pwd`

# this might be an issue with root
export HOME=$WORKDIR

####################################################################################################
# initialize BAMBU
####################################################################################################

# setting up the software
setupCmssw $cmsswVersion

# prepare the python config from the given templates
cat $MIT_PROD_DIR/$CONFIG/$VERSION/${PY}.py \
    | sed "s@XX-LFN-XX@$lfn@g" \
    | sed "s@XX-GPACK-XX@$GPACK@g" \
    > $WORKDIR/${PY}.py

# getting our input (important: xrdcp needs cvmfs to be setup)
cd $WORKDIR; pwd; ls -lhrt
voms-proxy-info -all
downloadFile $GPACK $LFN
if ! [ -e "./$GPACK.root" ]
then
  echo " EXIT -- download failed."
  exit 1
fi

####################################################################################################
# run BAMBU
####################################################################################################

# run bambu making
cd $WORKDIR
pwd
ls -lhrt
executeCmd time cmsRun ${PY}.py

# this is a little naming issue that has to be fixed
mv bambu-output-file-tmp*.root  ${GPACK}_tmp.root

# cleanup the input
rm -f ./$GPACK.root

if ! [ -e "${GPACK}_tmp.root" ]
then
  echo " ERROR -- bambu production failed. No output file: ${GPACK}_tmp.root"
  echo "          EXIT now because there is no BAMBU file."
  exit 1
fi

showDiskSpace

####################################################################################################
# push our files out to the Tier-2 / Dropbox
####################################################################################################

cd $WORKDIR
pwd
ls -lhrt

# define base output location
REMOTE_SERVER="se01.cmsaf.mit.edu"
REMOTE_BASE="srm/v2/server?SFN=/mnt/hadoop/cms/store"
REMOTE_USER_DIR="/user/paus/filefi/$VERSION"

sample=`echo $GPACK | sed 's/\(.*\)_nev.*/\1/'`

# this is somewhat overkill but works very reliably, I suppose
#setupCmssw 7_6_3 # should work with new releases... right?
tar fzx $MIT_PROD_DIR/tgz/copy.tgz
pwd=`pwd`
for file in `echo ${GPACK}*`
do
  # always first show the proxy
  voms-proxy-info -all
  # now do the copy
  executeCmd time ./cmscp.py \
    --middleware OSG --PNN $REMOTE_SERVER --se_name $REMOTE_SERVER \
    --inputFileList $pwd/${file} \
    --destination srm://$REMOTE_SERVER:8443/${REMOTE_BASE}${REMOTE_USER_DIR}/${TASK}/${CRAB} \
    --for_lfn ${REMOTE_USER_DIR}/${TASK}/${CRAB}
done

# make condor happy because it also might want some of the files
executeCmd mv $WORKDIR/*.root $BASEDIR/

# leave the worker super clean

testBatch
if [ "$?" == "1" ]
then
  cd $BASEDIR
  executeCmd rm -rf $WORKDIR *.root
fi

# create the pickup output file for condor

echo " ---- D O N E ----" > $BASEDIR/${GPACK}.empty

pwd
ls -lhrt
echo " ---- D O N E ----"

exit 0
