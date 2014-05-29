#!/bin/bash
#---------------------------------------------------------------------------------------------------
#
# Script to integrate an externally produced AOD(SIM) sample into the Bambu production environment.
# This means first it is copied to our Tier-2 space, then it is cataloged so we can create a
# database entry for the lfns. After the sample now exists the production framework will pick it up
# and make a bambu sample out of it using the specified MIT_DATASET name.
#
#                                                                              C.Paus (May 25, 2014)
#---------------------------------------------------------------------------------------------------
# Original location:
# /store/user/arapyan/mc/ \
#  DoublyChargedHiggsToWWToLL_M-160_TuneZ2_8TeV-madgraph/Winter12_DR53X-PU_S10_START53_V7C-v1/AODSIM
# Target location:
# /store/user/paus/mc/ \
#  DoublyChargedHiggsToWWToLL_M-160_TuneZ2_8TeV-madgraph/Winter12_DR53X-PU_S10_START53_V7C-v1/AODSIM
#
# book:        mc
# dataset: \
#  DoublyChargedHiggsToWWToLL_M-160_TuneZ2_8TeV-madgraph/Winter12_DR53X-PU_S10_START53_V7C-v1/AODSIM
# mit_dataset: w13-hppmm160ww4l-aa-v1
# eos:         /eos/cms/store/user/arapyan
# pausmc:      /mnt/hadoop/cms/store/user/paus
#
# integrateExternalSample.sh mc \
#  DoublyChargedHiggsToWWToLL_M-160_TuneZ2_8TeV-madgraph/Winter12_DR53X-PU_S10_START53_V7C-v1/AODSIM \
#  w13-hppmm160ww4l-aa-v1 /eos/cms/store/user/arapyan /mnt/hadoop/cms/store/user/paus
#
# /store/user/arapyan/mc/DoublyChargedHiggsToWWToLL_M-200_TuneZ2_8TeV-madgraph
# /store/user/arapyan/mc/DoublyChargedHiggsToWWToLL_M-400_TuneZ2_8TeV-madgraph
# /store/user/arapyan/mc/DoublyChargedHiggsToWWToLL_M-600_TuneZ2_8TeV-madgraph
# /store/user/arapyan/mc/DoublyChargedHiggsToWWToLL_M-800_TuneZ2_8TeV-madgraph
# /store/user/arapyan/mc/VHToChi1Chi1_TuneZ2_8TeV-madgraph/
#
#---------------------------------------------------------------------------------------------------

#===================================================================================================
function checkForCondor {
  #
  # function that echos how many download jobs are running for a given dataset

  dataset="$1"

  jobs=`condor_q -global $USER -format "%s " Cmd -format "%s \n" Args | grep $dataset \
        | grep downloadFiles.sh | wc -l`
  echo $jobs
}

#===================================================================================================
function waitForCondor {
  #
  # function waits for all condor download jobs of a given dataset to complete

  dataset="$1"

  startTime=$(date +%s)
  nowTime=$(date +%s)
  duration=$(($nowTime - $startTime))

 jobs=$(checkForCondor $dataset)
  while [ $jobs -gt 0 ]
  do
    echo " waiting since  $duration sec  == condor queue has  $jobs jobs  left"
    sleep 60
    echo ""
    jobs=$(checkForCondor $dataset)
    nowTime=$(date +%s)
    duration=$(($nowTime - $startTime))
  done
  echo " Queues are empty ($jobs) -- Moving on now."
  echo ""
}

#===================================================================================================
function checkDatabase {
  #
  # function that echos the submit command for a given dataset if it is in the database

  dataset="$1"

  entry=`findSamples.py --cmssw=mc --pattern=$dataset --complete 2> /dev/null | grep submit`
  echo $entry
}

#===================================================================================================
# M A I N
#===================================================================================================
# command line arguments
BOOK="$1"
DATASET="$2"
MIT_DATASET="$3"
EOS="$4"
PAUSMC="$5"

# for file cataloging
cataBook=`dirname $DATASET`
cataDataset=`basename $DATASET`

# test whether download already running
jobs=$(checkForCondor $cataDataset)
if [ "$jobs" -lt 1 ]
then
  echo " EXE - downloadSample.sh $EOS $BOOK $DATASET $PAUSMC 100 ~/download 1"
  downloadSample.sh $EOS $BOOK $DATASET $PAUSMC 100 ~/download 1
else
  echo ""
  echo " Looks like download is already ongoing (nJobs: $jobs). Move on."
  echo ""
fi

# declare it in the Production.{mc|data} file
echo ""
echo "/$BOOK/$DATASET $MIT_DATASET 1 new -"
echo ""
echo ""
entry=$(checkDatabase $MIT_DATASET)
if [ "$entry" == "" ]
then
  read -p "Please update the Production.mc file first! [return] " yn
else
  echo " Found entry: $entry"
fi

# make sure not to start before condor is done
waitForCondor $cataDataset

# make a catalog
echo " EXE - catalog.sh -ce -m $BOOK cataDBook $cataDataset --retry"
catalog.sh -ce -m $BOOK $cataBook $cataDataset --retry

# when all is done (careful these files are used for checking, so no more changes once you start)
echo " EXE - generateLfns.py --rawFile=/home/cmsprod/catalog/t2mit/$BOOK/$DATASET/RawFiles.00 \
       > ~/cms/jobs/lfns/${MIT_DATASET}.lfns"
generateLfns.py --rawFile=/home/cmsprod/catalog/t2mit/$BOOK/$DATASET/RawFiles.00 \
       > ~/cms/jobs/lfns/${MIT_DATASET}.lfns

# submit the production (make sure not to overwrite your produced lfns --useExistingLfns)
echo " EXE - submit.py --mitCfg=filefi --version=032 --cmssw=mc --useExistingLfns --mitDataset=$MIT_DATASET"
findSamples.py --cmssw=mc --useExistingLfns --pattern=$MIT_DATASET --complete --exe

exit 0
