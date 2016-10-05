#!/bin/bash
#----------------------------------------------------------------------------------------------
# Merge POG-developped branches that are not in the release yet.
# Review the script for each Bambu release!
#----------------------------------------------------------------------------------------------

echo "*********************"
echo " MitProd/bin/setup.sh"
echo "*********************"

install-pkg() {
  GITGROUP=$1
  REF=$2
  shift 2
  PKGS="$@"

  cd /tmp
  git clone -n https://github.com/$GITGROUP/cmssw.git
  cd cmssw
  git fetch origin
  git checkout $REF $PKGS
  mv * $CMSSW_BASE/src/
  cd ..
  rm -rf cmssw
}

install-pkg cms-met origin/fromCMSSW_8_0_20_postICHEPfilter RecoMET/METFilters
install-pkg cms-btv-pog BoostedDoubleSVTaggerV3-WithWeightFiles-v1_from-CMSSW_8_0_8_patch1 RecoBTag/SecondaryVertex

cd $CMSSW_BASE/src

scram b -j12
