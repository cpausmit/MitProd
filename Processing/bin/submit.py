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
import task,translator,color
t2user = os.environ['TIER2_USER']

SRMSRC='/usr/bin'

#===================================================================================================
def domain():
    d = os.uname()[1]
    f = d.split('.')
    return '.'.join(f[1:])

def storage(seFile):
    # decide on the forseen default storage place (where are we running?)
    storageTag = 'T2_US_MIT'
    dom = domain()
    if   re.search('mit.edu',dom):
        storageTag = 'T2_US_MIT'
    elif re.search('cern.ch',dom):
        storageTag = 'T0_CH_CERN'
    cmd = 'grep ^' + storageTag + ' ' + seFile
    for line in os.popen(cmd).readlines():   # run command
        #print ' LINE: ' + line
        line = line[:-1]                     # strip '\n'
        line = line.replace(' ','')
        f = line.split(':')
        storageEle    = f[1] 
        storagePath   = f[2] 
        userRemoteDir = f[3] 
    return storageEle + ':' + storagePath + ':' + userRemoteDir

#===================================================================================================
def getFiles(mitCfg,version):
    # Prepare file based processing input
    runFile  =  os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'run.sh'
    if not os.path.exists(runFile):
        cmd = "Run file not found: %s" % runFile
        raise RuntimeError, cmd
    cmd = 'cp ' + runFile + ' ./'
    os.system(cmd)
    writeCfgFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + 'wCfg.py'
    cmd = 'cp ' + writeCfgFile + ' ./'
    if not os.path.exists(writeCfgFile):
        cmd = "Write Cfg file not found: %s" % writeCfgFile
        raise RuntimeError, cmd
    os.system(cmd)
    storageFile  =  os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' \
                   + 'storageTarget'
    if not os.path.exists(storageFile):
        cmd = "Storage file not found: %s" % storageFile
        raise RuntimeError, cmd
    cmd = 'cp ' + storageFile + ' ./'
    os.system(cmd)

#===================================================================================================
def makeLfnFile(mitCfg,version,mitDataset,dbs,useExistingLfns):
    lfnFile  = 'lfns/' + mitDataset + '.lfns'

    # give notice that file already exists
    if os.path.exists(lfnFile):
        print "\n INFO -- Lfn file found: %s. Someone already worked on this dataset.\n" % lfnFile

    # remove what we need to to start clean
    cmd = 'rm -f ' +  lfnFile + '-TMP'
    os.system(cmd)
    if not useExistingLfns:
        cmd = 'rm -f ' + lfnFile
        os.system(cmd)
    
    # recreate if requested or not existing
    if not useExistingLfns or not os.path.exists(lfnFile):
        cmd = 'input.py --dbs=' + dbs + ' --option=lfn --dataset=' + cmsDataset \
              + ' | sort -u > ' + lfnFile + '-TMP'
        print ' Input: ' + cmd
        os.system(cmd)

    # move the new file into the proper location
    if os.path.exists(lfnFile + '-TMP'):
        cmd = 'mv ' + lfnFile + '-TMP ' + lfnFile
        print ' Move: ' + cmd + '\n'
        os.system(cmd)

    return lfnFile

#===================================================================================================
def makeSiteFile(mitDataset,dbs,useExistingSites):
    siteFile  = './sites/' + mitDataset + '.sites'
    if os.path.exists(siteFile):
        print "\n INFO -- Site file found: %s. Someone already worked on this dataset.\n" % siteFile
        if not useExistingSites:
            cmd = 'rm ' + siteFile
            os.system(cmd)
    
    # recreate if requested or not existing
    if not useExistingSites or not os.path.exists(siteFile):
        cmd = 'sitesList.py --dbs=' + dbs + ' --dataset=' + cmsDataset + ' > ' + siteFile
        print ' Sites: ' + cmd + '\n'
        os.system(cmd)

    return siteFile

#===================================================================================================
def searchReplace(line,mitCfg,version,mitDataset,storage, \
                  cmsDataset='X',cmsswPy='X',dbs='X',sched='X',blacklist='X',skpEvts='X'):
    # decode storage variable
    st = storage.split(':')
    # compile search and replacement sequences
    pCmsDset   = re.compile('XX-CMSDATASET-XX')
    pMitDset   = re.compile('XX-MITDATASET-XX')
    pCmsswPy   = re.compile('XX-CMSSWPY-XX')
    pMitCfg    = re.compile('XX-MITCFG-XX')
    pMitVers   = re.compile('XX-MITVERSION-XX')
    pMitSe     = re.compile('XX-MITSE-XX')
    pMitSPath  = re.compile('XX-MITSPATH-XX')
    pMitRDir   = re.compile('XX-MITRDIR-XX')
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
    line = pMitSe    .sub(st[0],       line);
    line = pMitSPath .sub(st[1],       line);
    line = pMitRDir  .sub(st[2],       line);
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
def findStoragePath(mitCfg,version,mitDataset,seFile):
    # find the forseen storage place
    cmd = 'grep ^storage_path ' + os.environ['MIT_PROD_DIR'] \
          + '/' + mitCfg + '/' + version + '/crab.cfg'
    for file in os.popen(cmd).readlines():
        line         = file[:-1]
        line         = searchReplace(line,mitCfg,version,mitDataset,storage(seFile));
        # decode the storage directory name
        names        = line.split("=")
        names        = names[1:]
        storagePath  = "=".join(names)
        storagePath  = re.sub("\s", "",storagePath)
    cmd = 'grep ^user_remote_dir ' + os.environ['MIT_PROD_DIR'] \
          + '/' + mitCfg + '/' + version + '/crab.cfg'
    for file in os.popen(cmd).readlines():
        line         = file[:-1]
        line         = searchReplace(line,mitCfg,version,mitDataset,storage(seFile));
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
def createDirGeneral(storageEle,storagePath,fastCreate):
    # create all relevant subdirectories
    f = storagePath.split('/')                    # splitting every '/'
    storagePath2 = "/".join(f[:-1])
    storagePath1 = "/".join(f[:-2])
    storagePath0 = "/".join(f[:-3])

    # set the storage URL
    storageUrl = 'srm://' + storageEle + ':8443' + storagePath

    # check whether path exists
    cmd = SRMSRC + '/' + 'srmls ' + storageUrl + ' >& /dev/null'
    status = os.system(cmd)
    
    # create it only if missing
    if status == 0:
        print ' '
        print ' Directory already found.... moving on.'

    else:
        # create all relevant directories
        if fastCreate == 0:
            cmd = SRMSRC + '/' + 'srmmkdir srm://' + storageEle + ':8443' + storagePath0 + ' >& /dev/null'
            print '  srmmkdir: ' + cmd
            status = os.system(cmd)
            print '  srmmkdir: status %d'%(status)
            cmd = SRMSRC + '/' + 'srmmkdir srm://' + storageEle + ':8443' + storagePath1 + ' >& /dev/null'
            print '  srmmkdir: ' + cmd
            status = os.system(cmd)
            print '  srmmkdir: status %d'%(status)
            cmd = SRMSRC + '/' + 'srmmkdir srm://' + storageEle + ':8443' + storagePath2 + ' >& /dev/null'
            print '  srmmkdir: ' + cmd
            status = os.system(cmd)
            print '  srmmkdir: status %d'%(status)
        else:
            print ' Fast create activated. '
    
        # create the main storage directory
        cmd = SRMSRC + '/' + 'srmmkdir srm://' + storageEle + ':8443' + storagePath  + ' >& /dev/null'
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
    cmd = SRMSRC + '/' + 'srmls -l -count=1 ' + storageUrl + ' | grep UserPermission | grep RWX'
    status = -1
    for line in os.popen(cmd).readlines():  # run command
        line = line[:-1]
        print " Permissions? " + line
        status = os.system(cmd)
    print ' '

    if status != 0:
        print '  --> Directory creation or permissions failed. EXIT now.\n'
#        sys.exit(1)
 
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
usage += "                 --fixSites=<siteList>\n"
usage += "                 --complete\n"
usage += "                 --testJob\n"
usage += "                 --noTestJob\n"
usage += "                 --help\n"

# Define the valid options which can be specified and check out the command line
valid = ['cmsDataset=','mitDataset=','cmssw=','mitCfg=','version=','dbs=','sched=','blacklist=',
         'nSubmit=','skipEvents=','fixSites=','complete','useExistingLfns','useExistingSites',
         'testJob','noTestJob','test','help']
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# --------------------------------------------------------------------------------------------------
# Get all parameters for the production
# --------------------------------------------------------------------------------------------------
# crab id
cmd = "date +crab_0_%y%m%d_%H%M%S"
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    crabId = line
print ""
print " o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o"
print "\n This job will be CrabId: " + crabId + "\n"
print " o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o"
print ""
# other non command line parameters
fastCreate       = 0
# Set defaults for each command line parameter/option
cmsDataset       = None
mitDataset       = None
cmssw            = "cmssw"
mitCfg           = "filefi"
version          = os.environ['MIT_VERS']
#dbs              = "https://cmsdbsprod.cern.ch:8443/cms_dbs_prod_global/servlet/DBSServlet"
dbs              = "instance=prod/global"
#sched            = "glite"
sched            = "remoteGlidein"
blacklist        = "T1_TW_ASGC"
nSubmit          = -1
skpEvts          = ''
fixSites         = ''
complete         = 0
useExistingLfns  = False
useExistingSites = False
noTestJob        = 0
testJob          = 0
test             = 0

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
        cmssw = arg
    if opt == "--mitCfg":
        mitCfg = arg
    if opt == "--version":
        version = arg
    if opt == "--dbs":
        dbs = arg
    if opt == "--sched":
        sched = arg
    if opt == "--blacklist":
        blacklist = arg
    if opt == "--nSubmit":
        nSubmit = arg
    if opt == "--skipEvents":
        skpEvts = arg
    if opt == "--fixSites":
        fixSites = arg
    if opt == "--complete":
        complete = 1
    if opt == "--useExistingLfns":
        useExistingLfns  = True
    if opt == "--useExistingSites":
        useExistingSites = True
    if opt == "--noTestJob":
        noTestJob = 1
        testJob = 0
    if opt == "--testJob":
        testJob = 1
        noTestJob = 0
    if opt == "--test":
        test = 1

# Deal with obvious problems
if cmsDataset == None and mitDataset == None:
    cmd = "--cmsDataset | --mitDataset  options not provided. One of them is required."
    raise RuntimeError, cmd
seFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/'+ version + '/seTable'
if not os.path.exists(seFile):
    cmd = "Storage element table not found: %s" % seFile
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
cmsswPy  = cmssw + '_' + crabId + '.py'

# Prepare the ce/se translator
trans = translator.Translator(os.environ['MIT_PROD_DIR']+'/'+mitCfg+'/'+version+'/ceTable',
                              os.environ['MIT_PROD_DIR']+'/'+mitCfg+'/'+version+'/seTable',
                              os.environ['MIT_PROD_DIR']+'/'+mitCfg+'/'+version+'/preferredSites')

# Create the corresponding crab task
crabTask             = task.Task(crabId,cmsDataset,mitDataset,mitCfg,version,cmssw)
if crabTask.mitDataset == 'undefined' or crabTask.cmsDataset == 'undefined':
    print " ERROR - dataset not defined."
    sys.exit(1)
else:
    mitDataset = crabTask.mitDataset
    cmsDataset = crabTask.cmsDataset
    #dbs        = crabTask.dbs

# Deal with obvious problems
if cmsDataset == None or mitDataset == None:
    cmd = "--cmsDataset & --mitDataset  " + \
          "Have to be defined now (do you have the right database (Productions.<cmssw>)?"
    raise RuntimeError, cmd

getFiles(mitCfg,version)
lfnFile  = makeLfnFile(mitCfg,version,mitDataset,dbs,useExistingLfns)

crabTask.storagePath = findStoragePath(mitCfg,version,mitDataset,seFile)
crabTask.loadAllLfns(lfnFile)
crabTask.loadCompletedLfns()
crabTask.createMissingLfns(lfnFile,lfnFile + '_' + crabTask.tag)
# decide whether there are more jobs (lfns=files) to be submitted
if crabTask.nLfnMissing == 0:
    print ' All requested LFNs are available. EXIT now.'
    sys.exit()
# decide whether we always need to create the full directory tree
if crabTask.nLfnDone != 0:
    fastCreate = 1

# ok, looks like we will be submitting
siteFile = makeSiteFile(mitDataset,dbs,useExistingSites)

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

# Storage

# Parse template input and write the crab configuration file
fileInput  = open(crabFile,'r')
fileOutput = open("crab_" + crabTask.tag + ".cfg-Template",'w')
line = fileInput.readline()
while (line != ''):
    if line[0] != '#':
        line = searchReplace(line,mitCfg,version,mitDataset,storage(seFile), \
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
        line = searchReplace(line,mitCfg,version,mitDataset,storage(seFile), \
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
pattern2    = 'Total of '

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
            createDirGeneral(storageEle,storagePath,fastCreate)
            fastCreate = 1     # now the base directory exists so move to the fast create modes

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
            nJobsTotal = f[4]
            if nJobsTotal == '':
                nJobsTotal = f[2]
    # report
    print '  --> %s jobs created (%s).\n'%(nJobsTotal,tag)
    # and cleanup the temporary file for the subtask
    cmd = "rm -f " + mitDataset + ".lfns_" + tag
    os.system(cmd)

    # test something useful is going to happen
    if int(nJobsTotal) > 0:
        print ' More than zero (%d) jobs got created, go ahead and submit (%s).\n'\
              %(int(nJobsTotal),crabId)
    else:
        print ' Zero or less jobs got created: cleanup and EXIT.'
        cmd = 'rm -rf `echo *' + crabId + '*`'
        print ' Cleanup: ' + cmd + '\n'
        #os.system(cmd)
        sys.exit(0)
        
    # adjust arguments for official datasets (not for private copies)
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

        
        if fixSites != '':
            sites = fixSites
        else:
            if not os.path.exists(siteFile):
                cmd  = "sites.py --block=" + block + " --dbs=" + dbs
            else:
                cmd  = "cat " + siteFile + "| grep " + block + " | cut -d ':' -f2 | tr -d ' '"
                #print 'CMD: ' + cmd
            for line in os.popen(cmd).readlines():   # run command
                line = line[:-1]
                sites = line

                print " SITES: " + sites

                ## not needed anymore ## sites = trans.translateSes(sites)
                sites = trans.cleanSites(sites)
                sites = trans.selectPreferred()

        if sites == '':
            print '\n ERROR - no sites for this data block, do not submit.'
            print '           --> move on to the next block.\n'
            continue
        else:
            print '   Block ' + block + '  process:  %d  to  %d'%(minIdxs[idx],maxIdxs[idx]) + \
                  ' at\n        > ' + sites
        # block with different sites found
        if sites != lastSites:
            mergedSites  .append(sites)
            mergedBlocks .append(blocks [idx]) # only the first block with these sites is stored
            mergedMinIdxs.append(minIdxs[idx])
            mergedMaxIdxs.append(maxIdxs[idx])
            lastSites  = sites
            mergedIdx += 1
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

    nFail = 0
    nSuccess = 0
    print '\n Submit the merged blocks:'
    for block in mergedBlocks:
        print '   Merged Block ' + block + '  process:  %d  to  %d'\
              %(mergedMinIdxs[idx],mergedMaxIdxs[idx]) + \
              ' at\n        > ' + mergedSites[idx]
    
        nSubmit = '%d-%d'%(mergedMinIdxs[idx],mergedMaxIdxs[idx])
        if mergedMinIdxs[idx] == mergedMaxIdxs[idx]:
            nSubmit = '%d,%d'%(mergedMinIdxs[idx],100000000)
        #cmd = 'crab -submit %s -continue %s -GRID.ce_white_list=%s'%(nSubmit,tag,mergedSites[idx])
        cmd = 'crab -submit %s -continue %s -GRID.se_white_list=%s'%(nSubmit,tag,mergedSites[idx])
        print '  ' + cmd + '\n'
        status = os.system(cmd)

        # keep track of success (failure only after second try)
        if status == 0:
            nSuccess += 1

        retry = False
        while status != 0 and not retry:
            retry = True
            print ' Submission failed  (%s) --> retry once!'%(cmd)
            status = os.system(cmd)

            # keep track of success and failure
            if status == 0:
                nSuccess += 1
            else:
                nFail += 1

        # last action in the loop: increment the merged blocks
        idx += 1
    
    print '  Number of blocks submitted: %d' % nSubmission

    # cleanup in case of total failure
    col = color.color()
    print  "Submission summary:" \
        + col.OKBLUE+col.BOLD + " %d successes "%(nSuccess) + col.ENDC \
        + col.FAIL + " %d failures"%(nFail) \
        + col.ENDC

    if nSuccess == 0 and nFail > 0:
        cmd = "rm -rf crab_" + tag
        print " TOTAL FAILURE -- removing: " + tag
        os.system(cmd)
        


# and cleanup the temporary file for the task
cmd = "rm -f crab_" + crabTask.tag + ".cfg crab_" + crabTask.tag + ".cfg-Template " \
      + cmsswPy + ' ' + cmsswPy + 'c'
os.system(cmd)

print ' Done... keep watching it...'

sys.exit(0)
