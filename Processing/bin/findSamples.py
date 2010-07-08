#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to identify all samples which are kept in our database.
#
# Author: C.Paus                                                                (September 23, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

def findStartedDatasets(path):
    print " Collecting information over started samples"
    datasetList = []

    cmd = 'list ' + path
    for line in os.popen(cmd).readlines():   # run command
        line = line[:-1]                     # strip '\n'
        f    = line.split(" ")
        size = f[0]
        file = f[1]

        #if debug == 1:
        #    print ' Debug:: adding: ' + file + ' with size ' + size
        datasetList.append(file)

    return datasetList

def findOngoingDatasets(path):
    print " Collecting information over ongoing samples"
    datasetList = []

    cmd = 'cat crab_[0-9]_[0-9]*_[0-9]*/share/crab.cfg |grep ^user_remote_dir'
    for line in os.popen(cmd).readlines():   # run command
        line    = line[:-1]                  # strip '\n'
        f       = line.split("/")
        dataset = f[-1]
        if re.search('crab_0',dataset):
            dataset = f[-2]
        
        #if debug == 1:
        #    print ' Debug:: adding: ' + dataset '
        datasetList.append(dataset)

    return datasetList

def findCompletedDatasets(path):
    print " Collecting information over completed samples"
    datasetList = []

    cmd = 'cat ' + os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/Completed  2> /dev/null'
    for line in os.popen(cmd).readlines():   # run command
        line    = line[:-1]                  # strip '\n'
        dataset = line

        #if debug == 1:
        #    print ' Debug:: adding: ' + dataset '
        datasetList.append(dataset)

    return datasetList

def inList(file,list):
    for entry in list:
        if entry == file:
            return True
    return False

def cleanupCompletedList(ongoingDsetList,completedDsetList):
    print " Update completed list with ongoing list"

    for dataset in ongoingDsetList:
        if inList(dataset,completedDsetList):
            print ' -> removing fropm completed: ' + dataset
            cmd  = 'cat ' + mitCfg + '/' + version + '/Completed|grep -v ^' + dataset + '$ > C.bak'
            cmd += '; mv C.bak ' + mitCfg + '/' + version + '/Completed'
            os.system(cmd)

#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage  = "\nUsage: findSamples.py --mitCfg=<name>\n"
usage += "                      --version=<version>\n"
usage += "                      --cmssw=<name>\n"
usage += "                      --pattern=<name>\n"
usage += "                      --exe\n"
usage += "                      --noInfo\n"
usage += "                      --noDownload\n"
usage += "                      --forceCopy\n"
usage += "                      --debug\n"
usage += "                      --help\n\n"

# Define the valid options which can be specified and check out the command line
valid = ['mitCfg=','version=','cmssw=','pattern=',\
         'help','exe','noInfo','noDownload','forceCopy','debug']
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
mitCfg     = 'filefi'
version    = '014'
cmssw      = ''
pattern    = ''
cmsswCfg   = 'cmssw.cfg'
exe        = 0
noInfo     = False
noDownload = False
forceCopy  = False
debug      = False

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--mitCfg":
        mitCfg     = arg
    if opt == "--version":
        version    = arg
    if opt == "--cmssw":
        cmssw      = arg
    if opt == "--pattern":
        pattern    = arg
    if opt == "--exe":
        exe        = 1
    if opt == "--noInfo":
        noInfo     = True
    if opt == "--noDownload":
        noDownload = True
    if opt == "--forceCopy":
        forceCopy  = True
    if opt == "--debug":
        debug      = True

# Read parameters needed
crabFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'crab.cfg'
if not os.path.exists(crabFile):
    cmd = "Crab file not found: %s" % crabFile
    raise RuntimeError, cmd
cmsswFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + cmsswCfg
if not os.path.exists(cmsswFile):
    cmd = "Cmssw file not found: %s" % cmsswFile
    cmsswCfg = 'cmssw.py'
    cmsswFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + cmsswCfg
    if not os.path.exists(cmsswFile):
        cmd = "Cmssw file not found: %s" % cmsswFile
        cmd = " XXXX ERROR no valid configuration found XXXX"
        raise RuntimeError, cmd

# Find all started samples
path = '/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus/' + mitCfg + '/' + version
cmd  = 'grep ^storage_element ' + crabFile + '| grep cern.ch'
for line in os.popen(cmd).readlines():
    path = '/castor/cern.ch/user/p/paus/' + mitCfg + '/' + version

startedDsetList = findStartedDatasets(path)
#print " Dataset list: "
#for dataset in startedDsetList:
#    print ' -> ' + dataset

ongoingDsetList = findOngoingDatasets(path)
completedDsetList = findCompletedDatasets(path)
cleanupCompletedList(ongoingDsetList,completedDsetList)
completedDsetList = findCompletedDatasets(path)

# Resolve the other mitCfg parameters from the configuration file
cmd = 'cat '+ os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'Productions'
if cmssw != '':
    cmd = cmd + '.' + cmssw


print ''
join       = 0
mitDataset = ""
fullLine   = ""
bSlash     = "\\";
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    #print 'Line: "' + line + '"'
    # get ride of empty lines
    if line == '':
        continue
    # get ride of commented lines and read steering parameters
    if line[0] == '#':
        names = line.split()       # splitting every blank
        if len(names)> 2 and names[1] == 'crontab' and int(names[3]) != 1:
            print 'No crontab has been set.... (LINE: ' + line + ')'
            sys.exit(0)
        continue

    # join lines
    if join == 1:
        fullLine += line
    else:
        fullLine  = line

    # determine if finished or more is coming
    if fullLine[-1] == bSlash:
        join = 1
        fullLine = fullLine[:-1]
    else:
        join = 0
        fullLine = " ".join(str(fullLine).split()).strip()
        # test whether there is a directory   
        names      = fullLine.split()       # splitting every blank
        if debug == True:
            print "FullLine: " + fullLine
        cmsDataset = names[0]
        mitDataset = names[1]               # this is the equivalent MIT name of the dataset
        nevents    = int(names[2])          # number of events to be used in the production
        procStatus = names[3]
        local      = names[4]
        
        if pattern != '' and not re.search(pattern,mitDataset):
            continue

        cmd = 'submit.py --mitDataset=' + mitDataset + ' --mitCfg=' + mitCfg + \
              ' --version=' + version + ' --noTestJob'
        if cmssw != '':
            cmd = cmd + " --cmssw=" + cmssw

        # check for errors (to be done)

        # check for the logical combinations
        if   not inList(mitDataset,startedDsetList):
            
            #print ' new: ' + mitDataset
            print ' submitting: ' + cmd
            if exe == 1:
                os.system(cmd)

        elif     inList(mitDataset,ongoingDsetList):

            #print ' sub: ' + mitDataset
            print ' handled by jobSitter -- ' + mitDataset

        elif     inList(mitDataset,completedDsetList):
            if not noInfo:
                print ' don: ' + mitDataset
        else:

            cmd = cmd + ' --complete'
            #print ' toc: ' + mitDataset
            print ' completing: ' + cmd
            if exe == 1:
                os.system(cmd)

        # test download request
        if local != "-" and not noDownload:
             localPath  = local
             cmd = 'downloadSample.py --cmsDataset=' + cmsDataset + ' --mitCfg=' + mitCfg + \
                   " --version=" + version
             if cmssw != '':
                 cmd = cmd + " --cmssw=" + cmssw
             if forceCopy:
                 cmd += ' --forceCopy'
             print " " + cmd
             if exe == 1:
                 status = os.system(cmd)

##         if   procStatus == "new":
##             print " " + cmd
##             if exe == 1:
##                 status = os.system(cmd)
##         elif procStatus == "com":
##             cmd = cmd + ' --noTestJob --complete'
##             print " " + cmd
##             if exe == 1:
##                 status = os.system(cmd)
##         elif local != "-" and not noDownload:
##             localPath  = local
##             cmd = 'downloadSample.py --cmsDataset=' + cmsDataset + ' --mitCfg=' + mitCfg + \
##                   " --version=" + version
##             if forceCopy:
##                 cmd += ' --forceCopy'
##             print " " + cmd
##             if exe == 1:
##                 status = os.system(cmd)
##         else:
##             if not noInfo:
##                 print " Sample Info: " + fullLine
            
if mitDataset == "":
    print "ERROR - dataset not defined."
    sys.exit(0)

sys.exit(0)

# Say what we do now
print ' Preparing dataset: ' + cmsDataset + ' [MIT: ' + mitDataset + ' with ' + str(nevents) + \
      ' per job]'
