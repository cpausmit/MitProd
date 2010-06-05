#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Install the CMSSW and MIT (bambu) software for given versions of both softwares. We need a
# CMSSW_VERSION, MIT_VERS and an MIT_TAG.
#
#                 [ this is so far untested but would be nice to use ]
#
# Author: C.Paus                                                                      (June 4, 2010)
#---------------------------------------------------------------------------------------------------
ROOT=$HOME/cms/cmssw
CMSSW_VERSION=$1
export MIT_VERS=$2
export MIT_TAG=$3

# Check we have everything to proceed
if [ -z $CMSSW_VERSION ] || [ -z $MIT_VERS ] || [ -z $MIT_TAG ]
then
  echo ""
  echo " Define the necessary environment: CMSSW_VERSION, MIT_VERS and MIT_TAG"
  echo ""
  exit 1;
fi

# Install the general CMSSW release of the requested version locally
installCmsSw.sh $CMSSW_VERSION

# Prepare the MIT production environment
mkdir -p $ROOT/$MIT_VERS
cd       $ROOT/$MIT_VERS
source   $ROOT/../INIT $CMSSW_VERSION

# Install the MIT code according to the given tag
installMitTag.sh $MIT_TAG
