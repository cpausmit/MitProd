#!/bin/bash
# --------------------------------------------------------------------------------------------------
#
# Installation script for MitProd/Processing/agents. There will be lots of things to test and to
# fix, but this is the starting point. This installation has to be performed as user root.
#
#                                                                Ch.Paus: Version 0.0 (Apr 27, 2015)
# --------------------------------------------------------------------------------------------------
# Configuration parameters (this needs more work but for now)
export MIT_PROD_USER=cmsprod
export MIT_PROD_GROUP=zh

source agents/setupReview.sh

# make sure mysql is setup properly for server and clients otherwise this will not work check out
# the README

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
  # make sure to remove completely the previous installed software
  echo " Removing previous installation."
  rm -rf "$MIT_PROD_AGENTS_BASE"
fi
cp -r ./agents "$TRUNC"
chown ${MIT_PROD_USER}:${MIT_PROD_GROUP} -R $MIT_PROD_AGENTS_BASE

# create log/db structure
#========================
# owner has to be $MIT_PROD_USER:$MIT_PROD_GROUP, this user runs the process
mkdir -p $MIT_PROD_REVIEW_LOG
chown ${MIT_PROD_USER}:${MIT_PROD_GROUP} -R $MIT_PROD_REVIEW_LOG

# install and start daemons
#==========================

# stop potentially existing server process
if [ -e "/etc/init.d/reviewd" ]
then
  /etc/init.d/reviewd status
  /etc/init.d/reviewd stop
fi

# copy Detox daemon
cp $MIT_PROD_AGENTS_BASE/sysv/reviewd /etc/init.d/

# start new server
/etc/init.d/reviewd status
/etc/init.d/reviewd start
sleep 2
/etc/init.d/reviewd status

# start on boot
chkconfig --level 345 reviewd on

exit 0
