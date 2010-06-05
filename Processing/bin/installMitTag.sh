#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Install the software of a given MIT tag into the standard area. The MIT_VERS and CMSSW_BASE
# variables have to be already defined.
#
# Author: C.Paus                                                                      (June 4, 2010)
#---------------------------------------------------------------------------------------------------
MIT_TAG=$1

# Check we have everything to proceed
if [ -z $MIT_VERS ] || [ -z $CMSSW_BASE ] || [ -z $MIT_TAG ]
then
  echo ""
  echo " Define the necessary environment: MIT_VERS and CMSSW_BASE "
  echo ""
  exit 1;
fi

# Checkout, and compile
cd $CMSSW_BASE/src
cvs co -d MitCommon -r $MIT_TAG UserCode/MitCommon
cvs co -d MitEdm    -r $MIT_TAG UserCode/MitEdm
cvs co -d MitAna    -r $MIT_TAG UserCode/MitAna
cvs co -d MitProd   -r $MIT_TAG UserCode/MitProd

# Apply MIT specific patches
./MitAna/scripts/setup.sh

# Compile it all
cd $CMSSW_BASE/src
scram build -j 8
