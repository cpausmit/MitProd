#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to submit one complete production task
#
# Complete refers here to the proper preparation of the ultimate storage location (a storge element
# with a given storage path etc.) and the creation of the job configurations and the submission of
# the jobs to the grid sites. The grid is accessed via condor tools.
#
# Author: C.Paus                                                                     (June 16, 2016)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string
import condorTask

t2user = os.environ['T2TOOLS_USER']
SRMSRC='/usr/bin'

#===================================================================================================
def makeLfnFile(mitCfg,version,dataset,dbs,useExistingLfns):
    lfnFile  = './lfns/' + dataset + '.lfns'

    # give notice that file already exists
    if os.path.exists(lfnFile):
        print " INFO -- Lfn file found: %s. Someone already worked on this dataset." % lfnFile

    # remove what we need to to start clean
    cmd = 'rm -f ' +  lfnFile + '-TMP'
    os.system(cmd)
    if not useExistingLfns:
        cmd = 'rm -f ' + lfnFile
        os.system(cmd)
    
    # recreate if requested or not existing
    if not useExistingLfns or not os.path.exists(lfnFile):
        cmd = 'input.py --dbs=' + dbs + ' --option=lfn --dataset=' + dataset \
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
def makeSiteFile(dataset,dbs,useExistingSites):
    siteFile  = './sites/' + dataset + '.sites'
    if os.path.exists(siteFile):
        print " INFO -- Site file found: %s. Someone already worked on this dataset." % siteFile
        if not useExistingSites:
            cmd = 'rm ' + siteFile
            os.system(cmd)
    
    # recreate if requested or not existing
    if not useExistingSites or not os.path.exists(siteFile):
        cmd = 'sites.py --dbs=' + dbs + ' --dataset=' + dataset + ' > ' + siteFile
        print ' Sites: ' + cmd + '\n'
        os.system(cmd)

    return siteFile

#===================================================================================================
def createDir(storagePath):
    # create all relevant subdirectories
    print ' Faking it.... createDir'

    if True:
        return
    
 
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage =  "Usage: submit.py --dataset=<name>\n"
usage += "                 --cmssw=<name>\n"
usage += "                 --mitCfg=<name>\n"
usage += "                 --version=<version>\n"
usage += "                 --dbs=<name>\n"
usage += "                 --local  # submitting to the local scheduler (ex. t3serv015)\n"
usage += "                 --useExistingLfns\n"
usage += "                 --useExistingSites\n"
usage += "                 --help\n"

# Define the valid options which can be specified and check out the command line
valid = ['dataset=','cmssw=','mitCfg=','version=','dbs=',
         'local','useExistingLfns','useExistingSites',
         'help']
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# --------------------------------------------------------------------------------------------------
# Get all parameters for the production
# --------------------------------------------------------------------------------------------------
# condor id
cmd = "date +crab_0_%y%m%d_%H%M%S"
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    condorId = line
print ""
print " o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o"
print "\n This job will be CondorId: " + condorId + "\n"
print " o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o"
print ""

# Set defaults for each command line parameter/option
dataset = None
cmssw = "cmssw"
mitCfg = "filefi"
version = os.environ['MIT_VERS']
dbs = "prod/global"
local = False
useExistingLfns = False
useExistingSites = False

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--dataset":
        dataset = arg
    if opt == "--cmssw":
        cmssw = arg
    if opt == "--mitCfg":
        mitCfg = arg
    if opt == "--version":
        version = arg
    if opt == "--dbs":
        dbs = arg
    if opt == "--local":
        local = True
    if opt == "--useExistingLfns":
        useExistingLfns  = True
    if opt == "--useExistingSites":
        useExistingSites = True

# Deal with obvious problems
if dataset == None:
    cmd = "--dataset  required parameter not provided."
    raise RuntimeError, cmd
seFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/'+ version + '/seTable'
if not os.path.exists(seFile):
    cmd = "Storage element table not found: %s" % seFile
    raise RuntimeError, cmd
cmsswFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + cmssw + '.py'
if not os.path.exists(cmsswFile):
    cmd = "Cmssw file not found: %s" % cmsswFile
    cmd = " XXXX ERROR no valid configuration found XXXX"
    raise RuntimeError, cmd
cmsswPy  = cmssw + '_' + condorId + '.py'

# Make sure we know what to produce and where the input is
lfnFile = makeLfnFile(mitCfg,version,dataset,dbs,useExistingLfns)
siteFile = makeSiteFile(dataset,dbs,useExistingSites)

# Create the corresponding condor task
condorTask = condorTask.CondorTask(condorId,mitCfg,version,cmssw,dataset,dbs,lfnFile,siteFile)
if local:
    print ' Update Scheduler ....'
    condorTask.updateScheduler('t3serv015.mit.edu','cmsprod')
    condorTask.show()

# Prepare the environment
condorTask.createDirectories()
condorTask.makeTarBall()

# Make the submit file
condorTask.writeCondorSubmit()
 
# Submit this job
condorTask.condorSubmit()

# Make it clean
condorTask.cleanUp()

print ''
sys.exit(0)
