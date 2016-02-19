#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to cleanup the potentially bulky production output and create a job summary.
#
# Author: C.Paus                                                                  (February 8, 2010)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

# Make list of files to consider for cleaning
#--------------------------------------------
def makeFileList(crabId):

    allFiles = []
    cmd = 'find ./' + crabId + '/res \( -name \*.stderr -o -name \*.stdout \)'
    for line in os.popen(cmd).readlines():  # run command
        line       = line[:-1]              # strip '\n'
        file       = line                   # splitting every blank
        
        allFiles.append(file)

    return allFiles

# Cleanup one given file
#-----------------------
def cleanupFile(file):
    #print ' Cleaning file: ' + file 

    cmd = ' gzip ' + file
    status = os.system(cmd)
    
# Measure size of given directory
#--------------------------------
def sizeDirectoryMb(dir):
    cmd = 'du -s --block-size=1 ' + dir + ' | cut -f1'
    for line in os.popen(cmd).readlines():  # run command
        size = line[:-1]                    # strip '\n'
    return int(size)/1000./1000.
    
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage =  "Usage: cleanupLog.py --crabId=<name>\n"
usage += "                     --help\n"

# Define the valid options which can be specified and check out the command line
valid = ['crabId=','help']
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
crabId     = None

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--crabId":
        crabId = arg

# Deal with obvious problems
if crabId == None:
    cmd = "--crabId  required parameter not provided."
    raise RuntimeError, cmd

# Measure size before
sizeBeforeMb = sizeDirectoryMb(crabId)

# Produce the file list
allFiles = []
allFiles = makeFileList(crabId)

# Analyze each file
for file in allFiles:
    # cleanup the specific log files
    cleanupFile(file)

# Measure size after
sizeAfterMb = sizeDirectoryMb(crabId)

# Size difference
deltaMb      = (sizeBeforeMb-sizeAfterMb)

print ' '
print ' Space before:  %12.4f MB'%sizeBeforeMb
print ' Space after :  %12.4f MB'%sizeAfterMb
print ' ==================================='
print ' Gained      :  %12.4f MB'%deltaMb
print ' '
