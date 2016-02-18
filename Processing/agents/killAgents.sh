#!/bin/bash
# --------------------------------------------------------------------------------------------------
# Killing all active agents to start clean.
#
#                                                                Ch.Paus: Version 0.0 (Apr 27, 2015)
# --------------------------------------------------------------------------------------------------

function killAgent {
  # function that will install the daemon named as the given first parameter
  #   example: install reviewd

  # command line parameter
  daemon="$1" 
  process="$2"

  # stop potentially existing server process
  if [ -e "/etc/init.d/${daemon}" ]
  then
    /etc/init.d/${daemon} status
    /etc/init.d/${daemon} stop
  fi

  killall $daemon
  killall $process  
  list=`ps auxw |grep cmsprod | grep -v grep | grep jobSitter| tr -s ' '| cut -d ' ' -f2`
  if [ "$list" != "" ]
  then
    kill $list
  fi

  ps auxw |grep cmsprod | grep -v grep | grep $daemon  
  ps auxw |grep cmsprod | grep -v grep | grep $process

}
#---------------------------------------------------------------------------------------------------
#                                               M A I N
#---------------------------------------------------------------------------------------------------
# Setup the environment
source setupAgents.sh

# kill all daemons
#=================

killAgent reviewd  reviewRequests.py
killAgent catalogd jobSitter.py
killAgent cleanupd jobSitter.py
killAgent uploadd  dbxApi.sh

exit 0
