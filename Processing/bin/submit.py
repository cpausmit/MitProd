#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to test, create and submit one complete production task
#
# Complete refers here to the proper preparation of the ultimate storage location (a storge element
# with a given storage path etc.), the submission of a test job to evaluate timing and data output
# size and finally the creation of the job configurations and the submission of the jobs to the grid
# sites. The grid is accessed via crab tools.
#
# While the script is pretty complete it has become a bit long and messy. Some cleanup will
# hopefully soon be performed.
#
# Author: C.Paus                                                                      (July 1, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string
import task,translator

#===================================================================================================
def searchReplace(line,mitCfg,version,mitDataset, \
                  cmsDataset='X',cmsswPy='X',dbs='X',sched='X',blacklist='X',skpEvts='X'):
    # compile search and replacement sequences
    pCmsDset   = re.compile('XX-CMSDATASET-XX')
    pMitDset   = re.compile('XX-MITDATASET-XX')
    pCmsswPy   = re.compile('XX-CMSSWPY-XX')
    pMitCfg    = re.compile('XX-MITCFG-XX')
    pMitVers   = re.compile('XX-MITVERSION-XX')
    pDbs       = re.compile('XX-DBS-XX')
    pSched     = re.compile('XX-SCHED-XX')
    pBlacklist = re.compile('XX-BLACKLIST-XX')
    pSkpEvts   = re.compile('XX-SKIPEVTS-XX')
    # perform all search and replaces
    line = pCmsDset  .sub(cmsDataset,  line);
    line = pMitDset  .sub(mitDataset,  line);
    line = pCmsswPy  .sub(cmsswPy,     line);
    line = pMitCfg   .sub(mitCfg,      line);
    line = pMitVers  .sub(version,     line);
    line = pDbs      .sub(dbs,         line);
    line = pSched    .sub(sched,       line);
    line = pBlacklist.sub(blacklist,   line);
    return line

#===================================================================================================
def adjustCfg(line,nevents,crabId):
    # compile search and replacement sequences
    pNevents   = re.compile('XX-NEVENTS-XX')
    pCrabId    = re.compile('XX-CRABID-XX')
    # perform all search and replaces
    line = pNevents  .sub(str(nevents),line);
    line = pCrabId   .sub(crabId,      line);
    return line

#===================================================================================================
def findStoragePath(mitCfg,version,mitDataset):
    # find the forseen storage place
    cmd = 'grep ^storage_path ' + os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/crab.cfg'
    for file in os.popen(cmd).readlines():
        line         = file[:-1]
        line         = searchReplace(line,mitCfg,version,mitDataset);
        # decode the storage directory name
        names        = line.split("=")
        names        = names[1:]
        storagePath  = "=".join(names)
        storagePath  = re.sub("\s", "",storagePath)
    cmd = 'grep ^user_remote_dir ' + os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/crab.cfg'
    for file in os.popen(cmd).readlines():
        line         = file[:-1]
        line         = searchReplace(line,mitCfg,version,mitDataset);
        # decode the storage directory name
        names        = line.split(" ")
        storagePath += names[-1]
    return storagePath

#===================================================================================================
def createDirCern(storagePath):
    # check whether path exists
    cmd = 'rfdir ' + storagePath + ' >& /dev/null'
    status = os.system(cmd)
    
    # create it if missing
    if status == 0:
        print ' Castor directory exists: ' + storagePath + '\n  --> Moving on.'
    else:
        print ' Castor directory needs to be created.'
        cmd = 'rfmkdir -p ' + storagePath
        status = os.system(cmd)
        if status == 0:
            print '  --> Created: ' + storagePath
            
        # always set the permissions
    cmd = 'rfchmod 777 ' + storagePath
    status = os.system(cmd)
    if status == 0:
        print '  --> Set permissions: 777\n'
    else:
        print '  --> Setting permissions failed. EXIT now.\n'
        sys.exit(1)

#===================================================================================================
def createDirGeneral(storageEle,storagePath):
    # create all relevant subdirectories
    f = storagePath.split('/')                    # splitting every '/'
    storagePath2 = "/".join(f[:-1])
    storagePath1 = "/".join(f[:-2])
    storagePath0 = "/".join(f[:-3])

    # set the storage URL
    storageUrl = 'srm://' + storageEle + ':8443' + storagePath

    # check whether path exists
    cmd = 'srmls ' + storageUrl + ' >& /dev/null'
    status = os.system(cmd)
    
    # create it only if missing
    if status == 0:
        print ' '
        print ' Directory already found.... moving on.'

    else:
        # create all relevant directories
        cmd = 'srmmkdir srm://' + storageEle + ':8443' + storagePath0 + ' >& /dev/null'
        print '  srmmkdir: ' + cmd
        status = os.system(cmd)
        print '  srmmkdir: status %d'%(status)
        cmd = 'srmmkdir srm://' + storageEle + ':8443' + storagePath1 + ' >& /dev/null'
        print '  srmmkdir: ' + cmd
        status = os.system(cmd)
        print '  srmmkdir: status %d'%(status)
        cmd = 'srmmkdir srm://' + storageEle + ':8443' + storagePath2 + ' >& /dev/null'
        print '  srmmkdir: ' + cmd
        status = os.system(cmd)
        print '  srmmkdir: status %d'%(status)
    
        # create the main storage directory
        cmd = 'srmmkdir srm://' + storageEle + ':8443' + storagePath  + ' >& /dev/null'
        print '  srmmkdir: ' + cmd
        status = os.system(cmd)
        print '  srmmkdir: status  %d'%(status)
        if status == 0:
            print ' '
            print ' Directory was created.'
        else:
            print ' '
            print ' '
            print ' '
            print ' ERROR - failed to create the Storage Area.'
            print ' '
            print ' '

    print ' Check permissions with:  srmls -l -count=1 ' + storageUrl + '\n'
    cmd = 'srmls -l -count=1 ' + storageUrl + ' | grep GroupPermission | grep RWX'
    status = -1
    for line in os.popen(cmd).readlines():  # run command
        line = line[:-1]
        print " Permissions? " + line
        status = 0
       #status = os.system(cmd)
    print ' '

    if status != 0:
        print '  --> Directory creation or permissions failed. EXIT now.\n'
        sys.exit(1)
 
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage =  "Usage: submit.py --cmsDataset=<name> | --mitDataset=<name>\n"
usage += "                 --cmssw=<name>\n"
usage += "                 --mitCfg=<name>\n"
usage += "                 --version=<version>\n"
usage += "                 --dbs=<name>\n"
usage += "                 --sched=<name>\n"
usage += "                 --blacklist=<name>\n"
usage += "                 --nSubmit=<submittedJobs>\n"
usage += "                 --skipEvents=<'nRunX:nEventY','nRunXX:nEventYY',...>\n"
usage += "                 --complete\n"
usage += "                 --testJob\n"
usage += "                 --noTestJob\n"
usage += "                 --help\n"

# Define the valid options which can be specified and check out the command line
valid = ['cmsDataset=','mitDataset=','cmssw=','mitCfg=','version=','dbs=','sched=','blacklist=',
         'nSubmit=','skipEvents=','complete','testJob','noTestJob','test','help']
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
cmsDataset = None
mitDataset = None
cmd = "date +crab_0_%y%m%d_%H%M%S"
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    crabId = line
print "\n This job will be CrabId: " + crabId + "\n"

cmssw      = "cmssw"
mitCfg     = "filefi"
version    = "014"
#dbs        = "https://cmsdbsprod.cern.ch:8443/cms_dbs_prod_global/servlet/DBSServlet"
dbs        = "http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet"
sched      = "glite"
blacklist  = ""
nSubmit    = -1
skpEvts    = ''
complete   = 0
noTestJob  = 0
testJob    = 0
test       = 0

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--cmsDataset":
        cmsDataset = arg
    if opt == "--mitDataset":
        mitDataset = arg
    if opt == "--cmssw":
        cmssw      = arg
    if opt == "--mitCfg":
        mitCfg     = arg
    if opt == "--version":
        version    = arg
    if opt == "--dbs":
        dbs        = arg
    if opt == "--sched":
        sched      = arg
    if opt == "--blacklist":
        blacklist  = arg
    if opt == "--nSubmit":
        nSubmit    = arg
    if opt == "--skipEvents":
        skpEvts    = arg
    if opt == "--complete":
        complete   = 1
    if opt == "--noTestJob":
        noTestJob  = 1
        testJob    = 0
    if opt == "--testJob":
        testJob    = 1
        noTestJob  = 0
    if opt == "--test":
        test       = 1

# Make sure we have the right 'database' and the right config file
database = 'Productions'
cmsswPy  = cmssw + '_' + crabId + '.py'
if cmssw != 'cmssw':
    database += '.' + cmssw

# Deal with obvious problems
if cmsDataset == None and mitDataset == None:
    cmd = "--cmsDataset | --mitDataset  options not provided. One of them is required."
    raise RuntimeError, cmd

crabFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'crab.cfg'
if not os.path.exists(crabFile):
    cmd = "Crab file not found: %s" % crabFile
    raise RuntimeError, cmd
cmsswFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + cmssw + '.py'
if not os.path.exists(cmsswFile):
    cmd = "Cmssw file not found: %s" % cmsswFile
    cmd = " XXXX ERROR no valid configuration found XXXX"
    raise RuntimeError, cmd


# Prepare the ce/se translator
translator = translator.Translator(os.environ['MIT_PROD_DIR']+'/'+mitCfg+'/'+version+'/ceTable',
                                   os.environ['MIT_PROD_DIR']+'/'+mitCfg+'/'+version+'/seTable')

# Resolve the other mitCfg parameters from the configuration file
cmd = 'cat ' + os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + database

join       = 0
fullLine   = ""
bSlash     = "\\";
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
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
        names      = fullLine.split()           # splitting every blank
        if names[0] == cmsDataset or names[1] == mitDataset:
            cmsDataset = names[0]               # CMS name of the dataset
            mitDataset = names[1]               # equivalent MIT name of the dataset
            nevents    = int(names[2])          # number of events to be used in the production
            if names[4] != "-":
                localPath  = names[4]
            print "\n Sample info from database  %s  for CMSSW config  %s\n   %s"\
                  %(database,cmsswPy,fullLine)
            if len(names) == 6:
                dbs = names[5]
                dbs = 'https://cmsdbsprod.cern.ch:8443/cms_dbs_' + dbs + '/servlet/DBSServlet'
                print '   dbs: ' + dbs + '\n'
            else:
                print ''

if mitDataset == None or cmsDataset == None:
    print "ERROR - dataset not defined."
    sys.exit(1)

# Prepare file based processing input
runFile  =  os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'run.sh'
if not os.path.exists(runFile):
    cmd = "Run file not found: %s" % runFile
    raise RuntimeError, cmd
cmd = 'cp ' + runFile + ' ./'
os.system(cmd)
writeCfgFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'writeCfg.py'
cmd = 'cp ' + writeCfgFile + ' ./'
os.system(cmd)

lfnFile  = mitCfg + '/' + version + '/' + mitDataset + '.lfns'
if os.path.exists(lfnFile):
    cmd = "Lfn file found: %s. This means someone already worked on this dataset." % lfnFile
####    raise RuntimeError, cmd
else:
    cmd = 'input.py --option=lfn --dataset=' + cmsDataset + ' > ' + lfnFile
    print ' Input: ' + cmd
    os.system(cmd)

# Create the corresponding crab task
crabTask             = task.Task(crabId,cmsDataset,mitCfg,version)
crabTask.storagePath = findStoragePath(mitCfg,version,mitDataset)
crabTask.loadAllLfns(lfnFile)
crabTask.loadCompletedLfns()
crabTask.createMissingLfns(lfnFile,lfnFile + '_' + crabTask.tag)
crabTask.createSubTasks(lfnFile + '_' + crabTask.tag)
cmd = 'cp ' + lfnFile +  '_' + crabTask.tag + '_*' + ' ./'
os.system(cmd)

nevents = len(crabTask.lfns)

# Say what we do now
print ' Preparing dataset: ' + cmsDataset + ' [MIT: ' + mitDataset + ' with ' + str(nevents) + \
      ' total jobs(=lfns), nEvtsTotal: %d]' % crabTask.nTotalEvts

# --------------------------------------------------------------------------------------------------
# Prepare the config files
# --------------------------------------------------------------------------------------------------
# Cleaning up
cmd = "rm -f crab_" + crabTask.tag + ".cfg crab_" + crabTask.tag + ".cfg-Template " + cmsswPy
os.system(cmd)

# Parse template input and write the crab configuration file
fileInput  = open(crabFile,'r')
fileOutput = open("crab_" + crabTask.tag + ".cfg-Template",'w')
line = fileInput.readline()
while (line != ''):
    if line[0] != '#':
        line = searchReplace(line,mitCfg,version,mitDataset, \
                             cmsDataset,cmsswPy,dbs,sched,blacklist,skpEvts)
    fileOutput.write(line)
    line = fileInput.readline()
fileInput .close()
fileOutput.close()

# Parse template input and write the crab configuration file
fileInput  = open(cmsswFile,'r')
fileOutput = open(cmsswPy,'w')
line = fileInput.readline()
while (line != ''):
    if line[0] != '#':
        line = searchReplace(line,mitCfg,version,mitDataset, \
                             cmsDataset,cmsswPy,dbs,sched,blacklist,skpEvts)
    fileOutput.write(line)
    line = fileInput.readline()
fileInput .close()
fileOutput.close()

# --------------------------------------------------------------------------------------------------
# Job creation and submission
# --------------------------------------------------------------------------------------------------
print '\n Using CMSSW version: ' + os.environ['CMSSW_VERSION']
print   ' Using CRAB  version: ' + os.environ['CRAB_VERS'] + '\n'

pattern1    = 'working directory'
pattern2    = 'job\(s\) can run on'

# Go through the crabTask and submit each subtask to the grid
for subTask in crabTask.subTasks:
    
    tag = crabTask.tag + '_' + subTask.tag()
    print ' Working on subtask: ' + tag

    nJobsTotal  = 0
    crabIdCheck = ''

    # adjust crab config
    fileInput  = open("crab_" + crabTask.tag + ".cfg-Template",'r')
    fileOutput = open("crab_" + crabTask.tag + ".cfg",'w')
    line = fileInput.readline()
    while (line != ''):
        if line[0] != '#':
            line = adjustCfg(line,subTask.nSubTaskLfn,tag)
        fileOutput.write(line)
        line = fileInput.readline()
    fileInput .close()
    fileOutput.close()

    # ----------------------------------------------------------------------------------------------
    # Deal with storage element area
    # ----------------------------------------------------------------------------------------------
    # find the forseen storage place
    cmd = 'grep ^storage_element crab_' + crabTask.tag + '.cfg-Template'
    for file in os.popen(cmd).readlines():   # run command
        line        = file[:-1]              # strip '\n'
        # decode the storage element name
        names       = line.split("=")        # splitting every '='
        storageEle  = names.pop()
        storageEle  = re.sub("\s", "",storageEle)
    cmd = 'grep ^storage_path crab_' + crabTask.tag + '.cfg-Template'
    for file in os.popen(cmd).readlines():   # run command
        line        = file[:-1]              # strip '\n'
        # decode the storage directory name
        names       = line.split("=")        # splitting every '='
        names       = names[1:]
        storagePath = "=".join(names)
        storagePath = re.sub("\s", "",storagePath)
    cmd = 'grep ^user_remote_dir crab_' + crabTask.tag + '.cfg-Template'
    for file in os.popen(cmd).readlines():   # run command
        line        = file[:-1]              # strip '\n'
        # decode the storage directory name
        names        = line.split(" ")        # splitting every '='
        storagePath += names[-1]
    storageUrl = 'srm://' + storageEle + ':8443' + storagePath

    storagePath = adjustCfg(storagePath,subTask.nSubTaskLfn,tag)
    storageUrl  = adjustCfg(storageUrl, subTask.nSubTaskLfn,tag)
    
    print ' StorageUrl: ' + storageUrl
    
    # Create storage area
    if test == 0: # for testing we do not create the directories
        if (storageEle == 'srm.cern.ch'): 
            createDirCern(storagePath)
        else:
            createDirGeneral(storageEle,storagePath)

#    cmd  = "crab -create -debug 3 -USER.ui_working_dir=" + tag + " | tee forDaniele "
    cmd  = "crab -create -cfg crab_" + crabTask.tag + ".cfg -USER.ui_working_dir=" + tag
    print '  -> ' + cmd
    if test != 0:
        cmd = 'echo ' + cmd
    print '  ' + cmd
    for line in os.popen(cmd).readlines():   # run command
        line = line[:-1]                     # strip '\n'
        print ' CRAB: ' + line
        if re.search(pattern1,line):
            f = line.split("/")
            crabIdCheck = f[-2]
        if re.search(pattern2,line):
            f = line.split(" ")
            nJobsTotal = f[1]
            if nJobsTotal == '':
                nJobsTotal = f[2]
    # report
    print '  --> %s jobs created (%s).\n'%(nJobsTotal,tag)
    # and cleanup the temporary file for the subtask
    cmd = "rm -f " + mitDataset + ".lfns_" + tag
    os.system(cmd)
    
    # adjust arguments
    cmd = 'input.py --db=' + lfnFile + '_' + tag + ' --option=xml --dataset=' + cmsDataset + \
          ' > ' + tag + '/share/arguments.xml'
    print '  update arguments: ' + cmd
    if test == 0:
        os.system(cmd)

    # loop through the file and determine the submission parameters
    block   = ''
    blocks  = []
    idx     = 0
    minIdxs = []
    maxIdxs = []
    
    fileInput = open(lfnFile + '_' + tag,'r')
    line = fileInput.readline()
    while (line != ''):
        line = line[:-1]
        if line[0] != '#':
            idx    += 1
            f       = line.split()
            block   = f[0]
            lfn     = f[1]
            nEvents = f[2]
            if len(blocks) == 0 or block != blocks[-1]:
                # new block found
                blocks .append(block)
                minIdxs.append(idx)
                maxIdxs.append(idx)
            else:
                maxIdxs[-1] = idx
        # keep the reading going
        line = fileInput.readline()
    fileInput .close()

    # merge blocks together if they use the same sites
    lastSites     = ''
    idx           = 0
    mergedIdx     = 0
    mergedBlocks  = []
    mergedSites   = []
    mergedMinIdxs = []
    mergedMaxIdxs = []
    print '\n Show the unmerged blocks:'
    for block in blocks:
        cmd  = "sites.py --block=" + block
        for line in os.popen(cmd).readlines():   # run command
            line = line[:-1]
            sites = line
        print '   Block ' + block + '  process:  %d  to  %d'%(minIdxs[idx],maxIdxs[idx]) + \
              ' at\n        > ' + sites
        # block with different sites found
        if sites != lastSites:
            lastSites = sites
            mergedSites  .append(sites)
            mergedBlocks .append(blocks[idx])
            mergedMinIdxs.append(minIdxs[idx])
            mergedMaxIdxs.append(maxIdxs[idx])
            mergedIdx =+ 1
        else:
            mergedMaxIdxs[mergedIdx-1] = maxIdxs[idx]
        # last action in the loop: increment the unmerged blocks
        idx += 1

    # Show already what we will do
    idx = 0
    print '\n Show the merged blocks:'
    for block in mergedBlocks:
        print '   Merged Block ' + block + '  process:  %d  to  %d'\
              %(mergedMinIdxs[idx],mergedMaxIdxs[idx]) + \
              ' at\n        > ' + mergedSites[idx]
        # last action in the loop: increment the merged blocks
        idx += 1

    # perfrom the submission block by block (using the merged blocks of course)
    nSubmission = len(mergedBlocks)
    idx = 0
    print '\n Submit the merged blocks:'
    for block in mergedBlocks:
        print '   Merged Block ' + block + '  process:  %d  to  %d'\
              %(mergedMinIdxs[idx],mergedMaxIdxs[idx]) + \
              ' at\n        > ' + mergedSites[idx]
    
        nSubmit = '%d-%d'%(mergedMinIdxs[idx],mergedMaxIdxs[idx])
        if mergedMinIdxs[idx] == mergedMaxIdxs[idx]:
            nSubmit = '%d,%d'%(mergedMinIdxs[idx],100000000)
        translatedSites = translator.translateSes(mergedSites[idx])
        cmd = 'crab -submit %s -continue %s -GRID.ce_white_list=%s'%(nSubmit,tag,translatedSites)
        print '  ' + cmd
        status = os.system(cmd)
        while status != 0:
            print ' Submission failed  (%s) --> retry'%(cmd)
            status = os.system(cmd)

        # last action in the loop: increment the merged blocks
        idx += 1
    
    print '  Number of blocks submitted: %d' % nSubmission


# and cleanup the temporary file for the task
cmd = "rm -f crab_" + crabTask.tag + ".cfg crab_" + crabTask.tag + ".cfg-Template " \
      + cmsswPy + ' ' + cmsswPy + 'c'
os.system(cmd)

print ' Done... keep watching it...'
sys.exit(0)

   
## if nSubmit != -1:
##     cmd = 'crab -continue %s -submit %s'%(crabId,nSubmit)
##     print 'SUBMIT.PY: ' + cmd
##     status = os.system(cmd)
##     if status == 0:
##         print '  --> job submitted\n'
## else:
##     leftOver   = int(nJobsTotal)
##     nSubmitted = 0
##     nSubBatch  = 80
##     while (nSubmitted < int(nJobsTotal)):
##         if leftOver < nSubBatch:
##             nSubBatch = leftOver
##         
##         cmd = 'crab -continue %s -submit %d'%(crabId,nSubBatch)
##         print 'SUBMIT.PY: ' + cmd
##         status = os.system(cmd)
##         if status == 0:
##             print '  --> %d job submitted\n'%(nSubBatch)
##         leftOver   = leftOver   - nSubBatch
##         nSubmitted = nSubmitted + nSubBatch
## 
## #print 'SUBMIT.PY: ' + cmd
## #status = os.system(cmd)
## #if status == 0:
## #    print '  --> job submitted\n'
## 

# --------------------------------------------------------------------------------------------------
# Run a test job to test the configuration and measure the expected output size
# --------------------------------------------------------------------------------------------------
if noTestJob == 0:
    #-----------------------------------------------------------------------------------------------
    # use a specific file for test
    #-----------------------------------------------------------------------------------------------
    # first check whether we already have a local file
    print '\n Try to find an existing local file using "find"'
    f    = cmsDataset.split("/")
    name = f[1]
    vers = f[2]
    tier = f[3]
    file = ""
    lfn  = ""
    cmd  = 'find ./store/ -name ' + name
    print 'Searching: ' + cmd
    for line in os.popen(cmd).readlines():  # run command
        file = line[:-1]                    # strip '\n'

    # looks like there could be a file we found a directory, confirm
    if file != "":
        print '\n We have a directory ' + file + ' .. confirming'
        file = file + '/' + tier
        cmd = 'find ' + file + ' -name \*.root'
        for line in os.popen(cmd).readlines():  # run command
            if line.find(vers):
                file = line[:-1]                # strip '\n'
                lfn = file[1:]

        if os.path.exists(file):
            print ' moving on with locally found file: \n' + '   ' + file
        else:
            print ' no local file found'
            file = ""
            lfn = ""

    # now try to see whether we can find a file to download
    if file == "":
        cmd = './bin/findLfn.py --input=' + cmsDataset + ' | grep /store/'
        print '\n Find an LFN to download: ' + cmd
        for line in os.popen(cmd).readlines():  # run command
            if line.find("/store") != -1:
                lfn = line[:-1]                 # strip '\n'
                break
        if lfn == "":
            print "\n WARNING: No file found, continue assuming it is a simulation job.\n\n"
        else:
            print '  --> LFN: ' + lfn
            file = '.' + lfn

            if os.path.exists(file):
                print '  --> File already exists: ' + file
            else:
                cmd = './bin/downloadLfn.py ' + lfn
                print '  --> downloading: ' + cmd
                status = os.system(cmd)
                if status != 0:
                    print ' ERROR - failed to copy LFN. EXIT now!'
                    sys.exit(1)

    # Parse template input and adjust the input file to the newly copied lfn
    fileInput  = open(cmsswPy,'r')
    fileOutput = open("test-"+cmsswPy,'w')
    line = fileInput.readline()
    while (line != ''):
        if line.find("file:") != -1:
            line = '"file:' + lfn[1:] + '"\n'
        fileOutput.write(line)
        line = fileInput.readline()
    fileInput .close()
    fileOutput.close()

    # Setting the number of events (hard coded in the file so far)
    nTryEvts = 1000.

    print '\n  --> Please wait, running a test job now! Should be short (trying %.0f'%nTryEvts + \
          ' evts). Check log: cmssw.log'
    cmd = 'rm -f cmssw.log; /usr/bin/time --format "%e %U %S" cmsRun test-' + cmsswPy + \
          ' >& cmssw.log'
    print ' CMD: ' + cmd
    status = os.system(cmd)

    cmd = 'tail -1 cmssw.log'
    for file in os.popen(cmd).readlines():  # run command
        line       = file[:-1]              # strip '\n'
        f          = line.split()           # splitting every blank
        rtime      = float(f[0])            # wall clock time
        utime      = float(f[1])            # user time
        stime      = float(f[2])            # system time

    nEvtsTest = 1000
    cmd = 'grep \'Begin processing\' cmssw.log | tail -1'
    for file in os.popen(cmd).readlines():  # run command
        line       = file[:-1]              # strip '\n'
        # test whether there is a directory
        f          = line.split()           # splitting every blank
        nEvtsTest  = f[3]                   # this is the number of records processed
        nEvtsTest  = int(nEvtsTest[:-2])    # strip 'th'
        cmd = 'ls -s ' + mitDataset  + '*.root'
    size = 0
    for file in os.popen(cmd).readlines():  # run command
        line       = file[:-1]              # strip '\n'
        f          = line.split()           # splitting every blank
        size      += int(f[0])/1000.        # size in MB

    cmd = 'tail -1 cmssw.log'
    for file in os.popen(cmd).readlines():  # run command
        line       = file[:-1]              # strip '\n'
        # get total, user and system times
        names      = line.split()           # splitting every blank

    if nEvtsTest != nTryEvts:
        print ' WARNING - Instead of %f  did  %d'%(nTryEvts,nEvtsTest)

    print ' '
    print ' Number of test events produced:  %d'%nEvtsTest
    print ' File size for all events:        %.2f MB'%size
    print ' Processing time for all events:  %.2f secs (u: %.2f s: %.2f)'%(rtime,utime,stime)
    print ' '
    print '  --> 1 event == %.2f secs'%(rtime/nEvtsTest)
    print '  --> 1.00 GB == %d events'%(nEvtsTest/size*1024.)
    print '  --> %.2f GB == %d events'%(nevents/(nEvtsTest/size*1024.),nevents)
    print ' '

if testJob == 1:
    print '\n Test job finished, stopping now.\n'
    sys.exit(0)

    
## # are we just completing an existing production? and is there something to complete?
## if complete == 1:
##     f = storagePath.split('=')
##     rfDir = f[-1]
##     #cmd = 'castorInventory.py --nJobs=%s %s | grep Missing'%(nJobsTotal,rfDir)
##     cmd = 'castorInventory.py --nJobs=%s %s | grep Missing'%(nJobsTotal,storageUrl)
##     #print ' CMD: ' + cmd
##     for line in os.popen(cmd).readlines():   # run command
##         line = line[:-1]                     # strip '\n'
##         f = line.split(':')
##         nSubmit = f[1].strip()
##         f = nSubmit.split(',')
##         if      len(f) == 0 or nSubmit == '':
##             print ' No more jobs left it seems, nSubmit=' + nSubmit
##             cmd = 'rm -rf ' + crabId
##             print ' Cleanup: ' + cmd + '\n\n'
##             status = os.system(cmd)
##             sys.exit(0)
##         elif len(f) == 1:
##             nInvalid = str(int(nJobsTotal) + 1000)
##             print ' One more jobs left, careful, adjusted, nSubmit=' + nSubmit
##             nSubmit = nSubmit + ',' + nInvalid
## 
##         ### nSubmit = ",".join(f[:-1])
##     print ' Missing jobs are: ' + nSubmit
## 
## sys.exit(0)