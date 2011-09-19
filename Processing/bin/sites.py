#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Simple interface to command line DBS to prepare my crabTask input files.
#---------------------------------------------------------------------------------------------------
import os,sys,types,string,getopt

# Define string to explain usage of the script
usage =  "Usage: site.py --block=<name>\n"
usage += "               [ --dbs=<name> ]\n"
usage += "               --help\n"
    
# Define the valid options which can be specified and check out the command line
valid = ['block=','dbs=','help']
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
block = None
dbs   = ''

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--block":
        block = arg
    if opt == "--dbs":
        dbs   = arg

# Deal with obvious problems
if block == None:
    cmd = "--block=  required parameter not provided."
    raise RuntimeError, cmd

#---------------------------------------------------------------------------------------------------
# main
#---------------------------------------------------------------------------------------------------
# find relevant site for this block
cmd  = "dbs search "
if dbs != '':
    cmd += " --url=" + dbs
cmd += " --query=\"find site where block=" + block + "\""
cmd += "| grep -v DBS | grep \\\."
sites = []
siteText = ''
for line in os.popen(cmd).readlines():
        line = line[:-1]
        sites.append(line)
        if siteText != '':
            siteText += ','
        siteText += line

print siteText
