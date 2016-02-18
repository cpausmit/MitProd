#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to get a quick overview how far the production has come.
#
# Author: C.Paus                                                                      (Feb 16, 2016)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string
import task,jobs

def makeFileList(crabId):
    # Make list of files to consider for cleaning

    allFiles = []
    cmd = 'find ./' + crabId + '/res \( -name \*.summary \)'
    for line in os.popen(cmd).readlines():  # run command
        line       = line[:-1]              # strip '\n'
        file       = line                   # splitting every blank
        
        allFiles.append(file)

    return allFiles

#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage = \
      "\nUsage: status.py [ --crabId=<id>  --help ]\n"

# Define the valid options which can be specified and check out the command line
valid = ['crabId=','help']
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
crabId = ''

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--crabId":
        crabId = arg

# Deal with obvious problems
if crabId == "":
    cmd = "--crabId  parameter not provided. This is a required parameter."
    raise RuntimeError, cmd

# --------------------------------------------------------------------------------------------------
# Here is where the real action starts -------------------------------------------------------------
# --------------------------------------------------------------------------------------------------

# Produce the file list to be analyzed
allFiles = makeFileList(crabId)

# Instantiate our summary
summary = jobs.JobLogsSummary(crabId)

# Read each file and add to the summary
for file in allFiles:
    jobLog = jobs.JobLog()
    jobLog.readFromSummaryFile(file)
    summary.addJobLog(jobLog)

# Produce summary
summary.show()
