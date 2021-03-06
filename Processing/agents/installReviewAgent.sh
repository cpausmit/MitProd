#!/bin/bash
# --------------------------------------------------------------------------------------------------
#
# Installation script for MitProd/Processing/agents. There will be lots of things to test and to
# fix, but this is the starting point. This installation has to be performed as user root.
#
#                                                                Ch.Paus: Version 0.0 (Apr 27, 2015)
# --------------------------------------------------------------------------------------------------

function install {
  # function that will install the daemon named as the given first parameter
  #   example: install reviewd

  # command line parameter
  daemon="$1"
  version="$2"

  # make sure directories exist
  mkdir -p $MIT_PROD_AGENTS_LOG/${daemon}
  chown ${MIT_PROD_USER}:${MIT_PROD_GROUP} -R $MIT_PROD_AGENTS_LOG

  # stop potentially existing server process
  if [ -e "/etc/init.d/${daemon}-${version}" ]
  then
    /etc/init.d/${daemon}-${version} status
    /etc/init.d/${daemon}-${version} stop
  fi
  
  # copy daemon by filling the template
  cat $MIT_PROD_AGENTS_BASE/sysv/${daemon}-template \
      | sed "s@XX-REVIEWD-XX@reviewd-${version}@" > /etc/init.d/${daemon}-${version}
  chmod 755 /etc/init.d/${daemon}-${version}
  
  # start new server
  /etc/init.d/${daemon}-${version} status
  /etc/init.d/${daemon}-${version} start
  sleep 2
  /etc/init.d/${daemon}-${version} status
  
  # start on boot
  chkconfig --level 345 ${daemon}-${version} on
}
#---------------------------------------------------------------------------------------------------
#                                               M A I N
#---------------------------------------------------------------------------------------------------
# Setup the environment
source setupAgents.sh

# Configuration parameters (this needs more work but for now)
export MIT_PROD_USER=cmsprod
export MIT_PROD_GROUP=zh

# General installation (you have to be in the directory of install script and you have to be root)

TRUNC=`dirname $MIT_PROD_AGENTS_BASE`
if ! [ -d "$TRUNC" ]
then
  mkdir -p "$TRUNC"
  chown ${MIT_PROD_USER}:${MIT_PROD_GROUP} -R $TRUNC
fi

# copy the software
#==================

if [ -d "$MIT_PROD_AGENTS_BASE" ]
then
  # make sure to remove completely the previous installed software of the same version number
  echo " Removing previous installation."
  rm -rf "$MIT_PROD_AGENTS_BASE"
fi
mkdir   $MIT_PROD_AGENTS_BASE
cp -r * $MIT_PROD_AGENTS_BASE/
chown ${MIT_PROD_USER}:${MIT_PROD_GROUP} -R $MIT_PROD_AGENTS_BASE

# create log/db structure
#========================
# owner has to be $MIT_PROD_USER:$MIT_PROD_GROUP, this user runs the process
mkdir -p $MIT_PROD_AGENTS_LOG
chown ${MIT_PROD_USER}:${MIT_PROD_GROUP} -R $MIT_PROD_AGENTS_LOG

# install and start daemons
#==========================

install reviewd $MIT_VERS

# install web pages
#==================

$MIT_PROD_AGENTS_BASE/html/install.sh reviewd

exit 0
