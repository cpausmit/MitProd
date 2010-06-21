#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to automatically download a MIT dataset to our local cluster
#
# The download of the MIT dataset is organized in accordance with the dataset production logic. In
# general it is allowed to download the dataset from any location of a properly configured storage
# element. The script will do all most obvious tests to ensure efficient and safe download. For
# performance reason a checksum is not calculated. This ommission is considered completely safe as
# failures will be identified in the analysis phase and the rare occasions will be more effective to
# fix by hand.
#
# At present the download proceeds in one thread (one file at a time) which for performance reasons
# might not be optimal.
#
# Author: C.Paus                                                                      (July 1, 2008)
#---------------------------------------------------------------------------------------------------
# Missing but desired features:
# + accounting of size of each file
# + accounting of locally available files (avoid copying already existing files)
# + determine full list of files before starting to copy
# + minimal success check of the copy
# + calculate total data volume (to copy, already copied etc.)
# + add feature to check the castor status
# - add time estimates and progressions for copies
# - multi downloads to enhance performance
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

def Seconds():
    for secs in os.popen('date +%s').readlines():
        secs = int(secs[:-1])
    return secs

def InSkipList(file,list):
    for entry in list:
        if entry == file:
            return True
    return False
    
def DecodeSrmLs(line):
    line = line.strip()
    f    = line.split(" ")
    size = f[0]
    f    = f[1].split("/")
    file = f.pop()
    list = [ size, file ]
    return list
    
def DecodeRfDir(line):
    line = line.strip()
    f    = line.split(" ")
    file = f.pop()
    size = f[4]
    list = [ size, file ]
    return list

def BuildFileList(cmd):
    isSrm = 1
    f = cmd.split(" ")
    if f[0] == 'rfdir':
        isSrm = 0

    fileList = {}
    
    if debug == 1:
        print ' Debug:: list: ' + cmd
    for line in os.popen(cmd).readlines():   # run command
        line = line[:-1]                     # strip '\n'
        #print ' Line: ' + line
        f = line.split(" ")
        ##if isSrm == 1:
        ##    f = DecodeSrmLs(line)
        ##else:
        ##    f = DecodeRfDir(line)
        size = f[0]
        file = f[1]
        f = file.split("/")
        file = f[-1]
        if debug == 1:
            print ' Debug:: adding: ' + file + ' with size ' + size
        fileList[file] = int(size)

    return fileList

def BuildStagedFileList(storagePath,allFileList,cacheFile):

    # initialize the basics
    fileList = {}
    f = storagePath.split("=");
    rfPath = f[-1]

    # if this is not castor, trick it and mark them as staged
    if not (re.search('/castor/',rfPath)):
        for file, size in allFileList.iteritems():
            fileList[file] = 'STAGED'
        return fileList

    # here we deal with castor
    if debug == 1:
        print ' Debug:: rfpath: ' + rfPath
    if os.path.exists(cacheFile) and noCache == 0:
        print ' Using the cached stager queries at  ' + cacheFile
    for file, size in allFileList.iteritems():
        fullFile = rfPath + '/' + file
        if debug == 1:
            print ' Debug:: full file name: ' + fullFile
        if os.path.exists(cacheFile) and noCache == 0:
            cmd = 'grep ' + file + ' ' + cacheFile
        else:
            cmd = 'stager_qry -M ' + fullFile
        fileList[file] = 'undefined'
        for line in os.popen(cmd).readlines():  # run command
            line = line[:-1]
            f = line.split(" ")
            if f[0] == fullFile:
                f = line.split(" ")
                status = f[-1]
                fileList[file] = status
                
    return fileList

def CacheStagedFileList(cacheFile,storagePath,stagedFileList):
    print ' Caching stager query status to ' + cacheFile
    f = storagePath.split("=");
    rfPath = f[-1]
    fileOutput = open(cacheFile,'w')
    for file, status in stagedFileList.iteritems():
        line = rfPath + '/' + file + ' xyz@castorns ' + status + '\n'
        fileOutput.write(line)
    fileOutput.close()

def CopyFile(storageEle,storagePath,storageUrl,file,localDir):
    deltaT = 0
    print '     working on file: ' + file + ' to ' + localDir + \
          '  (size: %d MB) '%(int(size)/1024/1024)
    if storageEle == 'srm-cms.cern.ch':
        f = storagePath.split("=");
        rfPath = f[-1]
        cpy  = 'rfcp ' + rfPath + '/' + file + ' ' + localPath + '/' \
               + mitCfg + '/' + version + '/' + mitDataset + '/' + file
        #print '     using rfcp.... ' + cpy
        #sys.exit(0)
    else:
        #storageUrl = 'srm://' + storageEle + ':8443' + storagePath
        cpy  = 'lcg-cp ' + storageUrl + '/' + file + ' file:////' + localPath + '/' \
               + mitCfg + '/' + version + '/' + mitDataset + '/' + file

    # Check whether the file size make sense (zero length files are probably not yet ready to
    # copy and will not be transfered
    if size < 1:
        print ' WARNING - file size is <1b. Probably this file is not yet ready. Stop copy.'
    else:
        if debug == 1:
            print ' Debug:: copy: ' + cpy
        start = Seconds()
        status = os.system(cpy)
        end = Seconds()
        deltaT = end - start
    
    return deltaT    

def StageFile(storagePath,storageUrl,file):
    print '     staging in file: ' + file
    if storageEle == 'srm-cms.cern.ch':
        f = storagePath.split("=");
        rfPath = f[-1]
        stg  = 'stager_get -M ' + rfPath + '/' + file
    else:
        #storageUrl = 'srm://' + storageEle + ':8443' + storagePath
        stg  = 'echo lcg-cp ' + storageUrl + '/' + file + ' file:////' + localPath + '/' \
               + mitCfg + '/' + version + '/' + mitDataset + '/' + file

    if debug == 1:
        print ' Debug:: stage: ' + stg
    status = os.system(stg)

#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage =  "Usage: downloadSample.py --cmsDataset=<name> | --mitDataset=<name>\n"
usage += "                         --mitCfg=<name>\n"
usage += "                         --version=<version>\n"
usage += "                         --cmssw=<name>\n"
usage += "                         --localStorageUrl=<name>\n"
usage += "                         --localPath=<dir>\n"
usage += "                         --skip=<file list>\n"
usage += "                         --backward\n"
usage += "                         --debug\n"
usage += "                         --help\n"

# Define the valid options which can be specified and check out the command line
valid = ['cmsDataset=','mitDataset=','mitCfg=','version=','cmssw=','pattern=','localStorageUrl=',
         'localPath=','noCache','skip=',
         'forceCopy','backward',
         'debug','help']
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
cmsDataset      = None
mitDataset      = None
skip            = ''
skipList        = []
mitCfg          = 'filefi'
version         = '014'
cmssw           = ''
blockLocal      = 0
localStorageUrl = ''
localPath       = '/server/02b/mitprod'
pattern         = ''
noCache         = 0
backward        = ''
forceCopy       = False
debug           = 0
cmsswCfg        = 'cmssw.cfg'

# Read new values from the command line
for opt, arg in opts:
    if opt == '--help':
        print usage
        sys.exit(0)
    if opt == '--cmsDataset':
        cmsDataset      = arg
    if opt == '--mitDataset':
        mitDataset      = arg
    if opt == '--mitCfg':
        mitCfg          = arg
    if opt == '--version':
        version         = arg
    if opt == '--cmssw':
        cmssw           = arg
    if opt == '--pattern':
        pattern         = arg
    if opt == '--localStorageUrl':
        localStorageUrl = arg
    if opt == '--localPath':
        blockLocal      = 1
        localPath       = arg
    if opt == '--skip':
        skip            = arg
        skipList        = skip.split(',')
    if opt == '--noCache':
        noCache         = 1
    if opt == '--backward':
        backward        = ' -r '
    if opt == '--forceCopy':
        forceCopy       = True
    if opt == '--debug':
        debug           = 1

# Deal with obvious problems
if cmsDataset == None and mitDataset == None:
    cmd = '--cmsDataset option not provided. This is required.'
    raise RuntimeError, cmd

crabFile  = mitCfg + '/' + version + '/' + 'crab.cfg'
if not os.path.exists(crabFile):
    cmd = 'Crab file not found: %s' % crabFile
    raise RuntimeError, cmd
cmsswFile = mitCfg + '/' + version + '/' + cmsswCfg
if not os.path.exists(cmsswFile):
    cmd = 'Cmssw file not found: %s' % cmsswFile
    cmsswCfg = 'cmssw.py'
    cmsswFile = mitCfg + '/' + version + '/' + cmsswCfg
    if not os.path.exists(cmsswFile):
        cmd = 'Cmssw file not found: %s' % cmsswFile
        cmd = ' XXXX ERROR no valid configuration found XXXX'
        raise RuntimeError, cmd

# Resolve the other mitCfg parameters from the configuration file
cmd = 'cat ' + mitCfg + '/' + version + '/' + 'Productions'
if cmssw != '':
    cmd = cmd + '.' + cmssw

join       = 0
if cmsDataset == None:
    cmsDataset = ''
else:
    mitDataset = ''
    
fullLine   = ''
bSlash     = '\\';
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    #print 'Line: "' + line + '"'
    # get ride of empty or commented lines
    if line == '' or line[0] == '#':
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
        # test whether there is a directory   
        names      = fullLine.split()       # splitting every blank
        #print "FullLine: " + fullLine
        #print "Datasets: " + mitDataset + ' -> ' + cmsDataset + "\n"
        if names[0] == cmsDataset:
            mitDataset = names[1]               # this is the equivalent MIT name of the dataset
            nevents    = int(names[2])          # number of events to be used in the production
            if names[4] != "-" and blockLocal == 0:
                localPath  = names[4]
            #print "\n Sample Info: " + fullLine + "\n"
            #print "\n Local path : " + localPath + ' -> ' + names[4] + "\n"
        if names[1] == mitDataset:
            cmsDataset = names[0]               # this is the equivalent CMS name of the dataset
            nevents    = int(names[2])          # number of events to be used in the production
            if names[4] != "-" and blockLocal == 0:
                localPath  = names[4]
            #print "\n Sample Info: " + fullLine + "\n"
            #print "\n Local path : " + localPath + ' -> ' + names[4] + "\n"

if mitDataset == "":
    print "ERROR - dataset not defined."
    sys.exit(0)

#cmd = 'grep ' + cmsDataset + ' ' + mitCfg + '/' + version + '/' + 'Productions'
#for file in os.popen(cmd).readlines():  # run command
#    line       = file[:-1]              # strip '\n'
#    # test whether there is a directory
#    names      = line.split()           # splitting every blank
#    mitDataset = names[1]               # this is the equivalent MIT name of the dataset
#    nevents    = int(names[2])          # number of events to be used in the production

# Say what we do now
print '\n Preparing dataset for transfer: ' + cmsDataset + ' [MIT: ' + mitDataset + ']\n'

# --------------------------------------------------------------------------------------------------
# Deal with storage element area
# --------------------------------------------------------------------------------------------------
pMitDset = re.compile('XX-MITDATASET-XX')
pMitCfg  = re.compile('XX-MITCFG-XX')
pMitVers = re.compile('XX-MITVERSION-XX')
# find the forseen storage place
crabFile  = mitCfg + '/' + version + '/' + 'crab.cfg'
cmd = 'grep ^storage_element ' + crabFile
for file in os.popen(cmd).readlines():   # run command
    line        = file[:-1]              # strip '\n'
    # decode the storage element name
    names       = line.split("=")        # splitting every '='
    storageEle  = names.pop()
    storageEle  = re.sub("\s", "",storageEle)
# Compile search and replacement sequences just for the path
cmd = 'grep ^storage_path ' + crabFile
for file in os.popen(cmd).readlines():   # run command
    line        = file[:-1]              # strip '\n'
    line        = pMitDset.sub(mitDataset,line);
    line        = pMitCfg .sub(mitCfg,    line);
    line        = pMitVers.sub(version,   line);
    # decode the storage directory name
    names       = line.split("=")        # splitting every '='
    names       = names[1:]
    storagePath = "=".join(names)
    storagePath = re.sub("\s", "",storagePath)
storageUrl = 'srm://' + storageEle + ':8443' + storagePath 

cmd = 'grep ^user_remote_dir ' + crabFile
for file in os.popen(cmd).readlines():   # run command
    line        = file[:-1]              # strip '\n'
    line        = pMitDset.sub(mitDataset,line);
    line        = pMitCfg .sub(mitCfg,    line);
    line        = pMitVers.sub(version,   line);
    # decode the storage directory name
    names       = line.split("=")        # splitting every '='
    names       = names[1:]
    userRemoteDir = "=".join(names)
    userRemoteDir = re.sub("\s","",userRemoteDir)
    userRemoteDir = re.sub("/XX-CRABID-XX","",userRemoteDir)

if userRemoteDir != '':
    storagePath += userRemoteDir
    storageUrl  += userRemoteDir

if localStorageUrl != '':
    storageEle  = ''
    storagePath = ''
    storageUrl  = localStorageUrl

print ' --> StorageUrl: ' + storageUrl

#---------------------------------------------------------------------------------------------------
# create the local storage area
#---------------------------------------------------------------------------------------------------
print ' Make local path: ' + localPath
localDir = localPath + '/' + mitCfg + '/' + version + '/' + mitDataset
mkd  = 'mkdir -p ' + localDir
status = os.system(mkd)

if status != 0:
    print ' ERROR - could not create local directory ' + localDir
    sys.exit(1)
    
print ' --> LocalDir:   ' + localDir

cmd  = 'df --block-size=1 ' + localDir + ' | tr -s \' \' | tail -1'
for line in os.popen(cmd).readlines():   # run command
    line = line.strip()
    f    = line.split(" ")
    if line[0:0] == '/' or line[0:4] == 'fuse':
        free = int(f[3])
    else:
        free = int(f[2])

#---------------------------------------------------------------------------------------------------
# create a list af all files to be copied
#---------------------------------------------------------------------------------------------------
cmd = ''
f    = storagePath.split('=')
path = f.pop()
cmd  = 'list ' + path + ' | grep root | sort ' + backward

##if storageEle == 'srm.cern.ch' or storageEle == 'srm-cms.cern.ch':
##    cmd  = 'rfdir ' + path + ' | grep root | tr -s \' \' | sort ' + backward
##else:
##    cmd  = 'list ' + path + ' | grep root | sort ' + backward
##    #cmd  = 'srmls ' + storageUrl + ' | grep root | sort ' + backward

if pattern != "":
    cmd += ' | grep ' + pattern

print ' Find file: ' + cmd
cacheFile      = '/tmp/.cache_' + mitDataset
allFileList    = BuildFileList(cmd)
stagedFileList = BuildStagedFileList(storagePath,allFileList,cacheFile)
cacheStaged    = CacheStagedFileList(cacheFile,storagePath,stagedFileList)

#cmd  = 'find ' + localPath + '/' + mitCfg + '/' + version + '/' + mitDataset + \
#       ' -maxdepth 1 -type f -printf "%s %f\n"'
print 'List: ' + cmd
cmd  = 'list ' + localPath + '/' + mitCfg + '/' + version + '/' + mitDataset + ' | grep root'
doneFileList = BuildFileList(cmd)

#---------------------------------------------------------------------------------------------------
# go through the lists: first check files are consistent, then copy the remaining files
#---------------------------------------------------------------------------------------------------
# initialize data volumes
b2G             = 1.0/(1024.*1024.*1024)
nTotal          = 0
totalDataVolume = 0
nDone           = 0
doneDataVolume  = 0

for file, size in allFileList.iteritems():
    nTotal += 1
    totalDataVolume += size
    if (file in doneFileList) and (doneFileList[file] == size):
        nDone += 1
        doneDataVolume += size

print ' '
print ' Summary of data volume\n'
print ' --> number of files to copy:     %8d  (total: %d) '%(nTotal-nDone,nTotal)
print ' --> volume to copy [GB]:         %8.2f  (total: %.2f) '%(b2G*(totalDataVolume-doneDataVolume), \
                                                          b2G*totalDataVolume)
print ' --> free volume    [GB]:         %8.2f  '%(b2G*free)
print ' '

if free*0.85 < (totalDataVolume-doneDataVolume):
    print ' ERROR - probably no enough space on volume. See above (some safety assumed)!'
    sys.exit(1)

for file, size in doneFileList.iteritems():
    if file in allFileList:
        #print ' --> file is done: ' + file
        if allFileList[file] != size:
            print ' ERROR - file sizes did not match: ' + file + \
                  ' [ local: %10d, remote: %10d ]'%(size,allFileList[file])
            sys.exit(1)
    else:
        print ' ERROR - file from done list is not in the all files list. File: ' + file
        sys.exit(1)

totalSizeMb = 0.
totalTimeSc = 0.
for file, size in allFileList.iteritems():
    if debug == 1:
        print ' Debug:: ' + file + ' -> size %d'%size 

    totalDataVolume += size
    if file in doneFileList:
        print ' --> done, size match: %10d - %s'%(size,file)
        doneDataVolume = +size
    else:
        if not InSkipList(file,skipList):
            print ' --> copying file:     %10d - %s (castor stat: %s)'% \
                  (size,file,stagedFileList[file])
            if stagedFileList[file] == "STAGED" or forceCopy:

                sizeMb = size/1024./1024.
                deltaT = CopyFile(storageEle,storagePath,storageUrl,file,localDir)
                if deltaT > 0:
                    print '     time required [sec]: %7d rate [MB/sec]: %9.3f'%\
                          (deltaT,sizeMb/deltaT)
                else:
                    print '     time required [sec]: %7d rate [MB/sec]: ?'%(deltaT)
                totalTimeSc += deltaT
                totalSizeMb += sizeMb
            else:
                print '     skipping file:    %s'%(stagedFileList[file])
                StageFile(storagePath,storageUrl,file)

        else:
            print ' --> skipping file:    %10d - %s'%(size,file)

print ''
if totalTimeSc > 0:
    print ' Performance:  volume copied [GB] %9.3f; time [sec] %9d;  -> rate  [MB/sec] %9.3f'%\
          (totalSizeMb/1024.,totalTimeSc,totalSizeMb/totalTimeSc)
else:
    print ' Performance:  volume copied [GB] %9.3f; time [sec] %9d;  -> rate  [MB/sec] ?'%\
          (totalSizeMb/1024.,totalTimeSc)
print ''
