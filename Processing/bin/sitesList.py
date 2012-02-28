#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Simple interface to command line DBS to prepare my crabTask input files.
#---------------------------------------------------------------------------------------------------
import os,sys,types,string,getopt

# Define string to explain usage of the script
usage =  "Usage: site.py --dataset=<name>\n"
usage += "               [ --dbs=<name> ]\n"
usage += "               --help\n"
    
# Define the valid options which can be specified and check out the command line
valid = ['dataset=','dbs=','help']
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# --------------------------------------------------------------------------------------------------
# Get all parameters for the production
# --------------------------------------------------------------------------------------------------
# Set defaults for each option
dataset = None
dbs     = ''

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--dataset":
        dataset = arg
    if opt == "--dbs":
        dbs     = arg

# Deal with obvious problems
if dataset == None:
    cmd = "--dataset=  required parameter not provided."
    raise RuntimeError, cmd

#---------------------------------------------------------------------------------------------------
# main
#---------------------------------------------------------------------------------------------------
# find relevant site for this dataset
cmd  = "dbs search "
if dbs != '':
    cmd += " --url=" + dbs
cmd += " --query=\"find block,site where dataset=" + dataset + "\""
cmd += "| grep -v DBS | grep \\\. | tr -s ' ' | sort -u "

# setup the variable
sites     = {}
siteText  = ''
lastBlock = ''
block     = ''
site      = ''
for line in os.popen(cmd).readlines():
        line  = line[:-1]
        f     = line.split(' ');
        block = f[0]
        site  = f[1]
        if block != lastBlock and lastBlock != '':
            sites[lastBlock] = siteText
            lastBlock        = block
            siteText         = site
        else:
            lastBlock = block
            if site != '':
                siteText = siteText + ',' + site
            else:
                siteText = site

# pick up the last block
sites[block] = siteText

# print each block and the sites that hold it in a comma seperate list 
for block,sites in sites.iteritems():
    print block + ' : ' + sites
