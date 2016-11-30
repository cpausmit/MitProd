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
import processing

## t2user = os.environ['T2TOOLS_USER']
user =  os.environ['USER']
SRMSRC='/usr/bin'
 
#===================================================================================================
# M A I N
#===================================================================================================
# Define string to explain usage of the script
usage =  "Usage: submitCondor.py --dataset=<name>\n"
usage += "                       --cmssw=<name>\n"
usage += "                       --mitCfg=<name>\n"
usage += "                       --version=<version>\n"
usage += "                       --dbs=<name>\n"
usage += "                       --local  # submitting to the local scheduler (ex. t3serv015)\n"
usage += "                       --useExistingLfns\n"
usage += "                       --useExistingSites\n"
usage += "                       --help\n"

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
py = "cmssw"
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
        py = arg
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
cmsswFile = os.environ['MIT_PROD_DIR'] + '/' + mitCfg + '/' + version + '/' + py + '.py'
if not os.path.exists(cmsswFile):
    cmd = "Cmssw file not found: %s" % cmsswFile
    cmd = " XXXX ERROR no valid configuration found XXXX"
    raise RuntimeError, cmd
cmsswPy  = py + '_' + condorId + '.py'

# Read all information about the sample
sample = processing.Sample(dataset,dbs,useExistingLfns,useExistingSites)

# Setup the scheduler we are going to use
scheduler = None
if local:
    scheduler = processing.Scheduler('t3serv015.mit.edu',user)
else:
    scheduler = processing.Scheduler('submit.mit.edu',user,'/local/d01/%s'%(user))

# Generate the request
request = processing.Request(scheduler,sample,mitCfg,version,py)

# Create the corresponding condor task
task = processing.Task(condorId,request)

# Prepare the environment
task.createDirectories()
task.makeTarBall()

# Quick analysis of ongoing failures and related logfile cleanup
taskCleaner = processing.TaskCleaner(task)
taskCleaner.logCleanup()

# Make the submit file
task.writeCondorSubmit()
 
# Submit this job
task.condorSubmit()

# Make it clean
task.cleanUp()

print ''
sys.exit(0)
