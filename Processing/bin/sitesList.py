#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Simple interface to command line DBS to prepare my crabTask input files.
#---------------------------------------------------------------------------------------------------
import os,sys,types,string,getopt

# adding the certificate
cert = "--cert ~/.globus/usercert.pem --key ~/.globus/userkey.pem "

# Define string to explain usage of the script
usage =  "Usage: sitesList.py --dataset=<name>\n"
usage += "                  [ --dbs=<name> ]\n"
usage += "                    --help\n"
    
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
dbs     = 'prod/global'
private = False

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

# is it a private production
f = dataset.split('/')
if f[1] == "mc":
    private = True
    #print ' Attention -- private dataset identified.'

#---------------------------------------------------------------------------------------------------
# main
#---------------------------------------------------------------------------------------------------
# handle private production first
if private:
    print dataset + '#00000000-0000-0000-0000-000000000000 : ' + 'T2_US_MIT'
    sys.exit()

# dbs 3 interface
# - find all relevant blocks
cmd = 'das_client ' + cert + ' --format=plain --limit=0 --query="block dataset=' + dataset \
    + ' instance=' + dbs + '"'
#print 'BLOCKS: ' + cmd
blocks = []
for line in os.popen(cmd).readlines():
    line  = line[:-1]
    f     = line.split(' ');
    block = f[0]
    blocks.append(block)

# - find all sites (se) for each block
sites = {}
for block in blocks:
    siteString = ''
    #cmd = 'das_client --format=plain --limit=0 --query="site block=' + block + ' | grep site.se"'
    cmd = 'das_client ' + cert + ' --format=plain --limit=0 --query="site block=' + block \
        + ' instance=' + dbs + '"'
    #print 'BLOCK: ' + cmd
    for line in os.popen(cmd).readlines():
        line = line[:-1]
        f    = line.split(' ');
        if siteString == '':
            siteString = f[0]
        else:
            siteString = f[0] + "," + siteString

    sites[block] = siteString

# print each block and the sites that hold it in a comma seperate list 
for block,sites in sites.iteritems():
    print block + ' : ' + sites
