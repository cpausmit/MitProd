#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to cleanup the potentially bulky production output
#
# Author: C.Paus                                                                  (February 8, 2010)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

# Make list of files to consider for cleaning
#--------------------------------------------
def makeFileList(crabId):
    print ' Make the file list for crab Id '
    allFiles = []
    cmd = 'find ./' + crabId + '/res \( -name \*.stderr -o -name \*.stdout \)'
    for line in os.popen(cmd).readlines():  # run command
        line       = line[:-1]              # strip '\n'
        #print ' LINE: ' + line
        file       = line                   # splitting every blank
        #file       = f.pop()
        
        allFiles.append(file)
        #crabTask.show()
    return allFiles

# Cleanup one given file
#-----------------------
def cleanupFile(file):
    print ' Cleaning file: ' + file 

    cmd = ' gzip ' + file
    status = os.system(cmd)

    #cmd = ' mv ' + file + ' ' + file + '.original'
    #status = os.system(cmd)
    #cmd = ' cat ' + file + '.original | grep -v \'Begin processing the\' > ' + file
    #status = os.system(cmd)
    #cmd = ' rm ' + file + '.original'
    #status = os.system(cmd)
    
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

cmd = 'du -s --block-size=1 ' + crabId + ' | cut -f1'
for line in os.popen(cmd).readlines():  # run command
    sizeBefore = line[:-1]              # strip '\n'

allFiles = []
allFiles = makeFileList(crabId)

for file in allFiles:
    #print ' Clean file: %s'%(file)
    cleanupFile(file)

cmd = 'du -s --block-size=1 ' + crabId + ' | cut -f1'
for line in os.popen(cmd).readlines():  # run command
    sizeAfter = line[:-1]               # strip '\n'

sizeBeforeMb = int(sizeBefore)/1024./1024.
sizeAfterMb  = int(sizeAfter)/1024./1024.
deltaMb      = (int(sizeBefore)-int(sizeAfter))/1024./1024.

print ' '
print ' Space before:  %12.4f MB'%sizeBeforeMb
print ' Space after :  %12.4f MB'%sizeAfterMb
print ' ==================================='
print ' Gained      :  %12.4f MB'%deltaMb
print ' '
