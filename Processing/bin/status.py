#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to get a quick overview how far the production has come.
#
# Author: C.Paus                                                                      (Feb 16, 2016)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string
import task,jobs

def failedLogCopyDir():
    # Return agent log directory or None if not defined

    return os.getenv('MIT_PROD_AGENTS_LOG')

def fileTrunc(file):
    # Find the file trunc dropping the last extension

    f = file.split('.')
    f.pop()
    trunc = '.'.join(f)

    return trunc

def makeFileList(crabId):
    # Make list of files to consider for cleaning

    allFiles = []
    cmd = 'find ./' + crabId + '/res \( -name \*.stdout \)'
    for line in os.popen(cmd).readlines():
        line = line[:-1]
        file = line
        
        allFiles.append(file)

    return allFiles

def copyLog(summary,file,logCopy):
    # Copy a given log file to the agent log area

    # this is only relevant for agent mode!
    if not logCopy:
        return

    # we are copying indeed
    trunc = fileTrunc(file)
    dir = logCopy + '/' + summary.version + '/' + summary.dataset + '/' + summary.crabId
    cmd = 'mkdir -p ' + dir
    #print ' CMD: ' + cmd
    os.system(cmd)
    cmd = 'cp ' + trunc + '.s* ' + dir
    #print ' CMD: ' + cmd
    os.system(cmd)

    return

def zip(crabId,operation):
    # Copy a given log file to the agent log area

    cmd = 'EMPTY'
    if   operation == 'unzip':
        cmd = 'gunzip ' + crabId + '/res/*.stdout.gz 2> /dev/null'
    elif operation == 'zip':
        cmd = 'gzip ' + crabId + '/res/*.stdout 2> /dev/null'
    else:
        print ' ERROR -- operation not defined: ' + operation
        
    if cmd != 'EMPTY':
        os.system(cmd)

    return

#===================================================================================================
#  M A I N
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

# first we have to unzip
zip(crabId,'unzip')

# Produce the file list to be analyzed
allFiles = makeFileList(crabId)

# Instantiate our summary
summary = jobs.JobLogsSummary(crabId)

# Set potential log file copy
logCopy = failedLogCopyDir()

# Read each file and add to the summary
for file in allFiles:

    # create jobLog
    jobLog = jobs.JobLog()
    jobLog.createFromLogFile(file)
    summary.addJobLog(jobLog) 

    # make full copy of failed job logs
    if not jobLog.isSuccess():
        copyLog(summary,file,logCopy)

# Produce summary
summary.show()

# finally we have to zip again
zip(crabId,'zip')
