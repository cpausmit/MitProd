#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to get a quick overview how far the production has come.
#
# Author: C.Paus                                                                      (July 1, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string
import task

#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage = \
      "\nUsage: status.py [ --dataset= --help ]\n"

# Define the valid options which can be specified and check out the command line
valid = ['dataset=','help']
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# --------------------------------------------------------------------------------------------------
# Get all parameters for this little task
# --------------------------------------------------------------------------------------------------
# Set defaults
dataset   = ''

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--dataset":
        dataset   = arg

# Deal with obvious problems
if dataset == "":
    cmd = "--dataset  parameter not provided. This is a required parameter."
    raise RuntimeError, cmd

# --------------------------------------------------------------------------------------------------
# Here is where the real action starts -------------------------------------------------------------
# --------------------------------------------------------------------------------------------------
dir = "/mnt/hadoop/cms/store/user/paus/filefi/013/"

cmd = 'list ' + dir + dataset + ' | grep root | wc '
os.system(cmd)
cmd = 'list ' + dir + dataset + '/crab_* | grep root | wc '
os.system(cmd)
cmd = 'wc lfns/' + dataset + '.lfns '
os.system(cmd)
