#!/usr/bin/python
#---------------------------------------------------------------------------------------------------
# Script to identify all samples which are kept in our database.
#
# Author: C.Paus                                                                (September 23, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string
from subprocess import call
import MySQLdb
import task

#---------------------------------------------------------------------------------------------------
# H E L P E R
#---------------------------------------------------------------------------------------------------

def findStartedDatasets(path,debug=0):
    # Make a list of all datasets that have at sometimes already been considered AKA: started

    if debug == 1:
        print " Collecting information over started samples"
    datasetList = []

    cmd = 'list ' + path
    if debug:
        print ' List: ' + cmd
    for line in os.popen(cmd).readlines():   # run command
        line = line[:-1]                     # strip '\n'
        f    = line.split(" ")
        size = f[0]
        file = f[1]

        if not inList(file,datasetList):
            if debug == 1:
                print ' Debug:: adding started: ' + file + ' with size ' + size
            datasetList.append(file)

    return datasetList

def findOngoingDatasets(path,debug=0):
    # Make a list of all datasets that are presently active and being worked on AKA: ongoing

    if debug == 1:
        print " Collecting information over ongoing samples"
    datasetList = []

    cmd = 'cat crab_[0-9]_[0-9]*_[0-9]*/share/crab.cfg |grep ^user_remote_dir'
    for line in os.popen(cmd).readlines():   # run command
        line    = line[:-1]                  # strip '\n'
        f       = line.split("/")
        dataset = f[-1]
        if re.search('crab_0',dataset):
            dataset = f[-2]

        if not inList(dataset,datasetList):
            if debug == 1:
                print ' Debug:: adding ongoing: ' + dataset
            datasetList.append(dataset)

    return datasetList

def inList(file,list):
    # Check whether a given entry is alreay in the list (careful this is slow string comparison)

    for entry in list:
        if entry == file:
            return True
    return False

def testEnvironment(mitCfg,version,cmssw,cmsswCfg):
    # Basic checks will be implemented here to remove the clutter from the main

    # Does the local environment exist?
    dir = './' + mitCfg + '/' + version
    if not os.path.exists(dir):
        cmd = "\n Local work directory does not exist: %s\n" % dir
        raise RuntimeError, cmd
 
    # Read parameters needed
    crabFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'crab.cfg'
    if not os.path.exists(crabFile):
        cmd = "Crab file not found: %s" % crabFile
        raise RuntimeError, cmd

    # Look for the standard CMSSW python configuration file (empty file is fine)
    cmsswFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + cmsswCfg
    if not os.path.exists(cmsswFile):
        cmd = "Cmssw file not found: %s" % cmsswFile
        cmsswCfg = 'cmssw.py'
        cmsswFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + cmsswCfg
        if not os.path.exists(cmsswFile):
            cmd  = " Cmssw file not found: %s\n" % cmsswFile
            cmd += " XXXX ERROR no valid configuration found XXXX"
            raise RuntimeError, cmd

def updateCacheDb(updateCacheDb,useCachedDb,mitCfg,version,cmssw):
    # Deal with the cache of the database - either update the cache or do not
    
    if useCachedDb:
        print " Using cached version: "
        cmd = 'ls -lhrt ./' + mitCfg + '/' + version + '/Productions.' \
              + cmssw
        rc = call(cmd.split(' '))
    else:                                                              # possibly remaking the cache
        # parameters for the update
        webServer = 'http://t3serv001.mit.edu/~cmsprod'
        tmpFile = '/tmp/Productions.' + cmssw

        # remove cache to keep it clean
        cmd = 'rm -f ' + tmpFile
        os.system(cmd)
        # Download up to date database file for productions
        cmd = 'wget ' + webServer + '/' + mitCfg + '/' + version + '/Productions.' + cmssw \
              + ' -O ' + tmpFile + ' 2> /dev/null'
        if debug:
            print ' WGET: ' + cmd
        os.system(cmd)
        if os.path.exists(tmpFile):
            if debug:
                print " Download of central database file worked (%s)."%(tmpFile)
        else:
            print "\n ERROR - Download of central database file failed. EXIT!\n"
            sys.exit(0)
        
        # Check whether something has changed
        cmd  = 'diff ' + tmpFile + ' ./' + mitCfg + '/' + version + '/Productions.' + cmssw
        rc = call(cmd.split(' '))
        if rc == 0:
            if debug:
                print " No difference in the local and central database files (rc=%d)."%(rc)
        else:
            print " Differences in central and local file found (rc=%d)."%(rc)
            if updateCacheDb:
                print " --> forcing update as requested."
                answer = 'y'
            else:
                answer = raw_input(" Overwrite local database with changes and continue? [y/N] ")
                
            # Check whether something has changed
            if answer == 'y' or answer == 'Y':
                cmd  = 'mv /tmp/Productions.' + cmssw + ' ./' + mitCfg \
                       + '/' + version + '/Productions.' + cmssw
                rc = call(cmd.split(' '))
                print ' Local database was overwritten.'
            else:
                print ' Local database *not* overwritten. Exit here.'
                sys.exit(0)

def findPath(mitCfg,version):
    # Find the path to where we store our samples

    # start with T2_US_MIT as default
    storageTag = 'T2_US_MIT'
    domain = task.Domain()
    if   re.search('mit.edu',domain):
        storageTag = 'T2_US_MIT'
    elif re.search('cern.ch',domain):
        storageTag = 'T0_CH_CERN'
    # make connection with our storage information
    seTable = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'seTable'
    if not os.path.exists(seTable):
        cmd = "seTable file not found: %s" % seTable
        raise RuntimeError, cmd
    # extract the path name
    cmd = 'grep ^' + storageTag + ' ' + seTable + ' | cut -d = -f2 | sed \'s# : ##\''
    path = ''
    for line in os.popen(cmd).readlines():
        path = line[:-1] +  '/' + mitCfg + '/' + version

    return path

#---------------------------------------------------------------------------------------------------
# M A I N
#---------------------------------------------------------------------------------------------------
# Define string to explain usage of the script
usage  = "\nUsage: findSamples.py --mitCfg=<name>\n"
usage += "                      --version=<version> [ default: MIT_VERS ]\n"
usage += "                      --cmssw=<name>\n"
usage += "                      --pattern=<name>\n"
usage += "                      --updateCachedDb\n"
usage += "                      --useCachedDb\n"
usage += "                      --useExistingLfns\n"
usage += "                      --useExistingSites\n"
usage += "                      --exe\n"
usage += "                      --debug\n"
usage += "                      --help\n\n"

# usage += "                      --download=<int: -1,0,1>\n"
# usage += "                      --status=<int: -1,0,1>\n"
# usage += "                      --remakeLfns=<int: -1,0,1>\n"
# usage += "                      --show=<int: 0,1>\n"
 
# Define the valid options which can be specified and check out the command line
valid = ['mitCfg=','version=','cmssw=','pattern=','download=','status=','remakeLfns=','show=', \
         'help','exe','updateCacheDb','useCachedDb','useExistingLfns','useExistingSites','debug']
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# Get all parameters for the production
# -------------------------------------
# Set defaults for each option
mitCfg           = 'filefi'
version          = os.environ['MIT_VERS']
cmssw            = ''
pattern          = ''
cmsswCfg         = 'cmssw.cfg'
exe              = 0
updateCachedDb   = False
useCachedDb      = False
useExistingLfns  = False
useExistingSites = False
# download         = -1
# status           = -1
# remakeLfns       = -1
# show             = 0
debug            = False

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--mitCfg":
        mitCfg = arg
    if opt == "--version":
        version = arg
    if opt == "--cmssw":
        cmssw = arg
    if opt == "--pattern":
        pattern = arg
    if opt == "--exe":
        exe = 1
    if opt == "--updateCachedDb":
        updateCachedDb = True
    if opt == "--useCachedDb":
        useCachedDb = True
    if opt == "--useExistingLfns":
        useExistingLfns = True
    if opt == "--useExistingSites":
        useExistingSites = True
    if opt == "--download":
        download = int(arg)
    if opt == "--status":
        status = int(arg)
    if opt == "--remakeLfns":
        remakeLfns = int(arg)
    if opt == "--show":
        show = int(arg)
    if opt == "--debug":
        debug = True

# Basic tests first
testEnvironment(mitCfg,version,cmssw,cmsswCfg)
updateCacheDb(updateCacheDb,useCachedDb,mitCfg,version,cmssw)

# Where is our storage?
path = findPath(mitCfg,version)

# Find all started samples
startedDsetList  = findStartedDatasets(path,debug)
ongoingDsetList  = findOngoingDatasets(path,debug)

# Access the database to determine all requests
db = MySQLdb.connect(read_default_file="/etc/my.cnf",read_default_group="mysql",db="Bambu")
cursor = db.cursor()
sql = 'select ' + \
      'Datasets.DatasetProcess,Datasets.DatasetSetup,Datasets.DatasetTier,' + \
      'RequestConfig,RequestVersion,RequestPy from Requests left join Datasets' + \
      ' on Requests.DatasetId = Datasets.DatasetId where RequestConfig="' + \
      mitCfg + '" and RequestVersion = "' + version + '" and RequestPy = "' + cmssw + '";'
if debug:
    print ' SQL: ' + sql

# Try to acceszs the database
try:
    # Execute the SQL command
    cursor.execute(sql)
    results = cursor.fetchall()      
except:
    print " Error (%s): unable to fetch data."%(sql)
    sys.exit(0)

# Take the result from the database and look at it
for row in results:
    process = row[0]
    setup = row[1]
    tier = row[2]

    datasetName = process + '+' + setup+ '+' + tier

    if pattern != '' and not re.search(pattern,datasetName):
        continue

    print '# Submit new dataset: ' + datasetName
    cmd = ' submit.py --cmssw=' + cmssw + ' --mitCfg=' + mitCfg + \
          ' --version=' + version

    if useExistingLfns:
        cmd += " --useExistingLfns"
    if useExistingSites:
        cmd += " --useExistingSites"

    # last thing to add is the dataset itself (nicer printing)
    cmd += ' --mitDataset=' + datasetName

    # check for errors (to be done)

    # make sure dataset is not yet being worked on
    if not inList(datasetName,ongoingDsetList):
        print ' submitting: ' + cmd
        if exe == 1:
            os.system(cmd)
    else:
        print ' already running.'

sys.exit(0)
