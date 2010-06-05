#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Install a given version of the CMSSW software locally at MIT.
#
#   ex.  installCmsSw.sh <version>, _ex._  installCmsSw 3_6_1
#
# Author: C.Paus                                                                      (June 4, 2010)
#---------------------------------------------------------------------------------------------------
# define the general setup
export VO_CMS_SW_DIR=/server/02a/cmsprod/cmssoft
export    SCRAM_ARCH=slc5_ia32_gcc434

# Read the environment
export MY_CMSSW_VERSION="$1"
if [ ".$MY_CMSSW_VERSION" == "." ]
then
  echo
  echo "Please specify the desired software version."
  echo
  echo "  usage: installCmsSw.sh <version>"
  echo "     ex. installCmsSw.sh 3_6_1"
  echo
  exit 1
fi

# get the apt setup
echo VO_CMS_SW_DIR $VO_CMS_SW_DIR
export aptInit=`echo $VO_CMS_SW_DIR/$SCRAM_ARCH/external/apt/*/etc/profile.d/init.sh`
echo
echo "AptInit setup:  $aptInit"
echo
echo "       starting in 4 seconds...."
echo
sleep 4
source $aptInit

# say what we do
echo
echo "Trying to install CMSSW version $MY_CMSSW_VERSION"
echo "to directory $VO_CMS_SW_DIR"
echo
echo "df -h $VO_CMS_SW_DIR"; df -h $VO_CMS_SW_DIR
echo
echo "       starting in 4 seconds...."
echo
sleep 4

# update and get installation
apt-get update
# check what we have to offer
# apt-cache search cmssw | grep ${MY_CMSSW_VERSION}

if [ "`echo ${MY_CMSSW_VERSION} | grep patch `" != "" ]
then
  apt-get install cms+cmssw-patch+CMSSW_${MY_CMSSW_VERSION}
else
  apt-get install cms+cmssw+CMSSW_${MY_CMSSW_VERSION}
fi

exit 0;
