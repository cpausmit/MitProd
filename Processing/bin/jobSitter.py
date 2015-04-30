#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to go through my crab jobs, get status and output and take some completion action if
# needed and desired.
#
# Author: C.Paus                                                                      (July 1, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string,time
import task

def removeCrabTask(crabTask):
        cmd = 'crab -kill all -continue ' + crabTask.tag + ' >& /dev/null; rm -rf ' + crabTask.tag
        print ' KILL and REMOVE task: ' + cmd
        status = os.system(cmd)
        return status

def updateExitStati(tag,status):
        # Make the file
        file = tag + '/res/CMSSW_%d.stdout'%(status.index)
        #print ' Analyzing file: ' + file
        cmd = 'cat ' + file
        if os.path.exists(file):
            for line in os.popen(cmd).readlines():  # run command
                line = line[:-1]
                # get ride of empty or commented lines
                if line == '' or line[0] == '#':
                    continue
                # no more unnecessary spaces
                line = " ".join(str(line).split()).strip()

                f = line.split(" ")

                if f[0] == 'EXECUTABLE_EXIT_STATUS':
                    status.exitCode   = int(f[2])
                if f[0] == 'StageOutExitStatus':
                    status.exitStatus = int(f[2])
    #else:
    #    print ' output file for this job not yet retrieved. '

def appendBlacklistSites(tag,failedlist,blacklist,exe):
    # Make the file
    file = tag + '/share/crab.cfg'
    # Find out whether there are any ce black listed
    ceBlacklist = ""
    # Get the original black list
    cmd = 'cat ' + file

    # finding the one line
    join       = 0
    fullLine   = ""
    bSlash     = "\\";

    # New Configuration file
    fileOutput = open(tag + '_crab.cfg','w')

    for line in os.popen(cmd).readlines():  # run command
        line = line[:-1]
        ##print 'Line: ' + line

        # get ride of empty or commented lines
        if line == '' or line[0] == '#':
            continue
        # no more unnecessary spaces
        line = " ".join(str(line).split()).strip()
        # join lines
        if join == 1:
            fullLine += line
        else:
            fullLine  = line
        # determine if finished or more is coming
        if fullLine[-1] == bSlash:
            join = 1
            fullLine = fullLine[:-1]
        # line really ended, so now look at the whole thing
        else:
            join = 0
            # test whether there is a directory
            names      = fullLine.split('=')       # splitting every blank
            ##print "FullLine: " + fullLine
            if names[0] == 'ce_black_list':
                ceBlacklist = fullLine
                fullLine += ',' + failedlist
                if blacklist != "":
                    fullLine = "ce_black_list=" + blacklist
                print "    new blacklist: " + fullLine

            fileOutput.write(fullLine + '\n')

    fileOutput.close()

    if ceBlacklist == '':
        print ' WARNING - site blacklisting did not work'

    cmd = 'mv ' + tag + '_crab.cfg ' + file
    print "\nACTION -- MOVE: " + cmd
    status = 0
    if exe == 1:
        status = os.system(cmd)
    else:
        ##status = os.system('cat ' + tag + '_crab.cfg')
        status = os.system('rm ' + tag + '_crab.cfg')
    return status

def removeJobRemainders(storageEle,storagePath,mitDataset,index,exe):
    fileMit = 'srm://' + storageEle + ':8443' + storagePath + '/' + mitDataset + \
              '_000_%d'%(index) + '.root'
    fileEdm = 'srm://' + storageEle + ':8443' + storagePath + '/' + mitDataset + \
              '-edm_%d'%(index) + '.root'
    cmd = 'srmrm ' + fileMit + ' ' + fileEdm + ' >& /dev/null'
    #print 'ACTION -- REMOVE: \n   srmrm ' + fileMit + '\n   srmrm ' + fileEdm + '\n'
    status = 0
    if exe == 1:
        # for now not execute this # status = os.system(cmd)
        print 'For now not removing file: ' + cmd
    return status

#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage = \
      "\nUsage: jobSitter.py [ --pattern= --apattern= --blacklist=" \
      +                      "  --status --kill --remove --help --backward --clean --extend --one" \
      +                      " --exe ]\n"

# Define the valid options which can be specified and check out the command line
valid = ['pattern=','apattern=','blacklist=','catalog=',
	 'help','backward','kill','remove','clean','exe','extend','one']
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
pattern   = ''
apattern  = ''
blacklist = ''
catalog   = 0
clean     = 0
kill      = 0
remove    = 0
exe       = 0
extend    = 0
one       = 0
backward  = ''

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--pattern":
        pattern   = arg
    if opt == "--apattern":
        apattern  = arg
    if opt == "--blacklist":
        blacklist = arg
    if opt == "--catalog":
        catalog   = int(arg)
    if opt == "--clean":
        clean     = 1
    if opt == "--one":
        one       = 1
    if opt == "--exe":
        exe       = 1
    if opt == "--extend":
        extend    = 1
    if opt == "--backward":
        backward  = ' -r '
    if opt == "--kill":
        kill      = 1
    if opt == "--remove":
        remove    = 1

# --------------------------------------------------------------------------------------------------
# Here is where the real action starts -------------------------------------------------------------
# --------------------------------------------------------------------------------------------------

# Find the list of crab tasks to babysit
crabTasks   = []
datasetList = []
cmd = 'find ./ -maxdepth 1 -name crab_0_\* |grep -v cfg | sort' + backward
print '\n=============================================================================='
print ' Summary of crab task list: \n'
for line in os.popen(cmd).readlines():  # run command
    line       = line[:-1]              # strip '\n'
    ## print ' LINE: ' + line
    f          = line.split('/')        # splitting every blank
    tag        = f.pop()

    crabTask   = task.Task(tag)

    #print 'Pattern: ' + pattern + '  tag: ' + crabTask.mitDataset
    if apattern != '' and re.search(apattern,crabTask.mitDataset):
        print '\n Skipping: ' + crabTask.mitDataset + '\n\n'
    if re.search(pattern,crabTask.mitDataset):
        crabTasks.append(crabTask)
        crabTask.show()

    if one == 1:
        break

# Process the crab tasks determined to be relevant in the last query
print '\n=============================================================================='
print ' Process crab task list\n'
i = 0
for crabTask in crabTasks:

    print '\n------------------------------------------------------------------------------'
    print '  --> PREPPING ' + crabTask.tag \
          + '\n       -> ' + crabTask.cmsDataset + '  (' + crabTask.mitDataset + ')'\
          + '\n       -> ' + crabTask.storageEle \
          + '\n       -> ' + crabTask.storagePath
    print '------------------------------------------------------------------------------\n'

    dataset     = crabTask.cmsDataset
    storageEle  = crabTask.storageEle
    storagePath = crabTask.storagePath

    if True:
	    f = storagePath.split("=")
	    path1 = f[1]
	    path0 = "/".join(path1.split("/")[:-1])
	    cmd = ' glexec chmod a+w ' + path0 + ' ' + path1
	    status = os.system(cmd)

    if kill == 1:
            crabTask.killAndRemove(1)
	    continue

    if remove == 1:
            crabTask.remove(1)
	    continue

    crabTask.loadAllLfns('lfns/' + crabTask.mitDataset + '.lfns')

    # make sure catalog is up to date
    f   = storagePath.split(" ")
    cmd = 'catalog.sh -ceg -m ' + crabTask.mitCfg + ' ' \
          + crabTask.mitVersion + ' ' + crabTask.mitDataset + '/' + crabTask.tag + ' --retry'

    if catalog == 3:
            cmd = 'catalog.sh -cegt -m ' + crabTask.mitCfg + ' ' + crabTask.mitVersion + \
                   ' ' + crabTask.mitDataset + '/' + crabTask.tag + ' --retry'
    if catalog == 4:
            cmd = 'catalog.sh -eg -m ' + crabTask.mitCfg + ' ' + crabTask.mitVersion + \
                   ' ' + crabTask.mitDataset + '/' + crabTask.tag + ' --retry'
    if catalog == 5:
            cmd = 'catalog.sh -ceg -m ' + crabTask.mitCfg + ' ' + crabTask.mitVersion + \
                   ' ' + crabTask.mitDataset + '/' + crabTask.tag + ' --remove'
    if catalog == 6:
            cmd = 'catalog.sh -e -m ' + crabTask.mitCfg + ' ' + crabTask.mitVersion + \
                   ' ' + crabTask.mitDataset + '/' + crabTask.tag + ' --compact'
    if catalog == 7:
            cmd = 'catalog.sh -g -m ' + crabTask.mitCfg + ' ' + crabTask.mitVersion + \
                   ' ' + crabTask.mitDataset + '/' + crabTask.tag + ' --compact'

    print '\n  --> CATALOG '
    print '      ' + cmd + '\n'
    
    if catalog != 0:
        os.system(cmd)

    # break out of the loop as only cataloging is required
    if catalog > 1:
        continue

    # check the status
    print '\n  --> STATUS ' + crabTask.tag + ' -- ' + crabTask.mitDataset

    # break out of the loop if request is too young
    ageMinutes = (time.time() - os.path.getmtime("./"+crabTask.tag)) / 60.
    minimumAgeMinutes = 60.
    if ageMinutes < minimumAgeMinutes:
        print ""
        print " Skipping this request. It is too fresh for now (<%.1f)."%(minimumAgeMinutes)
        print " age: %.1f min"%(ageMinutes)
        print ""
        continue

    # interact with crab to get the job status
    crabTask.getJobStati()
    if len(crabTask.jobStati) < 1:
        print ' ERROR - dropped empty crab task from the work list.'
        print '         crab task id: ' + crabTask.tag
        continue
    else:
        print ' '
        print ' Task status: ' + crabTask.status

        if crabTask.status == 'completed' or crabTask.status == 'finished':

            crabTask.remove(clean)
            #print " DEBUG crabTask.remove(clean) "

	    print ' INFO - crab task has been removed, continuing.\n'
	    continue
        print ' '

    # review failing sites
    siteList = ""
    if len(crabTask.failingSites) > 0:
        nSites = 0
        siteList = ",".join(crabTask.failingSites)
        print " Failing sites (consider blacklisting them)"
        for site,nAbort in crabTask.failingSites.iteritems():
            nSites += 1
            print '    ' + site + '(%d'%nAbort  + ')'
    appendBlacklistSites(crabTask.tag,siteList,blacklist,exe)

    # review all job stati and update exit stati if needed
    for status in crabTask.jobStati:
        if status.tag == 'Retrieved' and (status.exitCode < 0 and status.exitStatus < 0):
            updateExitStati(crabTask.tag,status)
            #status.showCompact()

    # review all job stati and propose action
    subList   = ''
    resubList = ''
    for status in crabTask.jobStati:
        ##print ' %4.0d '%(status.index) + ' --> ' + status.tag
        if ((status.tag == 'Created' and status.outputFile == 0)):
            status.showCompact()
            if subList == '':
                subList += '%d'%(status.index)
            else:
                subList += ',%d'%(status.index)
        if ((status.tag == 'Aborted'  or status.exitCode > 0 or status.exitStatus > 0)):
	##or(status.tag == 'Retrieved' and status.outputFile == 0)):
            status.showCompact()
            if resubList == '':
                resubList += '%d'%(status.index)
            else:
                resubList += ',%d'%(status.index)
            # for failed job first remove remainders before resubmitting
            if status.outputFile == 1:
                removeJobRemainders(storageEle,storagePath,crabTask.mitDataset,status.index,exe)

    if subList != '':
        if not re.search('-',subList) and not re.search(',',subList):
            subList = subList + ',999999999'
        cmd = 'crab -c ' + crabTask.tag + ' -submit ' + subList
    if resubList != '':
        cmd = 'crab -c ' + crabTask.tag + ' -resubmit ' + resubList


    cmd = 'crab -getoutput -continue ' + crabTask.tag
    print '\n  --> GETOUTPUT ' + crabTask.tag + ' -- ' + crabTask.mitDataset
    print '  --> ' + cmd
    status = os.system(cmd)

    cmd = 'cleanupLog.py --crabId ' + crabTask.tag
    status = os.system(cmd)

    i += 1
