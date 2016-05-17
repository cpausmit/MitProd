#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Script to extract catalog information from condor outputs.
#
# Catalog extraction principle:
#
# 1) create a raw file with the bare bones information in the cataloging directory
#    happens in this script extractCatalog.py: order files by the job numbers
#    - this raw file has to have various versions to ensure existing data does
#      not get overwritten or mixed in
#
# 2) from the raw file we will then in a separate step generate the catalog
#    - need to be able to deal with files being added on in the end
#
# 3) dealing with files added later on?!
#    - need to move output from batch job into done directory
#    - need to find a way to add on this information to the existing catalog
# 
# Author: C.Paus                                                                  (November 08 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,socket,getopt,re,string

def move(srcFile,source,target):
    # determine whether we are at MIT
    tier2 = re.search('cmsaf.mit.edu',target) 

    if not tier2:
        tier2 = re.search('/mnt/hadoop/cms/store',target) 

    # create the command and show it
    cmd = 'move ' + source + '  ' + target
    print cmd

    # at CERN execute right away for Tier-2 make an entry in the queue to be executed later at once
    status = 0
    if tier2:
        mvCmd = "echo \"mv  /mnt/hadoop/cms" + "/".join(source.split('/')[3:]) \
                +         ' /mnt/hadoop/cms' \
                +               "/".join(target.split('/')[3:]) \
                +  "; rm -f /mnt/hadoop/cms" + "/".join(source.split('/')[3:]) + "\" >> " + srcFile
        os.system(mvCmd)

        #status = os.system(cmd)
    else:
        status = os.system(cmd)

    return tier2

  
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage  = "\n"
usage += " Usage: extractCatalog.py --dataset=<name>\n"
usage += "                          --mitCfg=<name>\n"
usage += "                          --version=<name>\n"
usage += "                          --catalogDir=<name>\n"
usage += "                          --retry\n"
usage += "                          --remove\n"
usage += "                          --compact\n"
usage += "                          --test\n"
usage += "                          --retest\n"
usage += "                          --debug\n"
usage += "                          --help\n\n"

# Define the valid options which can be specified and check out the command line
valid = ['dataset=','mitCfg=','version=','catalogDir=','retry','remove','compact','test',
         'retest','debug','help']
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# global (are we working at the Tier-2)
tier2 = False

# --------------------------------------------------------------------------------------------------
# Get all parameters for the production
# --------------------------------------------------------------------------------------------------
# Set defaults for each option
server     = "srm://se01.cmsaf.mit.edu:8443/srm/managerv2?SFN="
catalogDir = "/home/cmsprod/catalog/t2mit"
dataset    = None
mitCfg     = "filefi"
version    = "023"
lRetry     = 0
lRemove    = 0
compact    = 0
test       = 0
debug      = 0
retest     = 0
official   = 0

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--catalogDir":
        catalogDir = arg
    if opt == "--dataset":
        dataset    = arg
    if opt == "--mitCfg":
        mitCfg     = arg
    if opt == "--version":
        version    = arg
    if opt == "--retry":
        lRetry     = 1
    if opt == "--remove":
        lRemove    = 1
    if opt == "--compact":
        compact    = 1
    if opt == "--test":
        test       = 1
    if opt == "--debug":
        debug      = 1
    if opt == "--retest":
        retest     = 1

# --------------------------------------------------------------------------------------------------
# Initialize the job according to the parameters
# --------------------------------------------------------------------------------------------------
# Deal with obvious problems
if dataset == None:
    cmd = "--dataset option not provided. This is required."
    raise RuntimeError, cmd

# See whether we are dealing with an official production request
if re.search('crab_0',dataset):
    official   = 1
    f = dataset.split('/')
    offDataset = f[0]
    crabId     = f[1]

    f = crabId.split('_')

    fileInput = None
    if len(f) == 5:
        try:
            fileInput = open(os.environ['HOME']+'/cms/jobs/lfns/'+offDataset +'.lfns'+'_'+crabId,'r')
        except IOError:
            print ' Could not open database for filename conversion -- FATAL \n   ' + \
                  os.environ['HOME']+ '/cms/jobs/lfns/'+offDataset+'.lfns'+'_'+crabId
            print ' Remove this directory and data therein ' + \
                  ' remove --exe ...\n'
            sys.exit(1)
    elif len(f) == 4:
        try:
            fileInput = open(os.environ['HOME']+'/cms/jobs/lfns/'+offDataset +'.lfns','r')
        except IOError:
            print ' Could not open database for filename conversion -- FATAL \n   ' + \
                  os.environ['HOME']+ '/cms/jobs/lfns/'+offDataset+'.lfns'
            print ' Remove this directory and data therein ' + \
                  ' remove --exe ...\n'
            sys.exit(1)
    else:
        sys.exit(1)
        
    
    index = 1
    files = {}
    nevts = {}
    nevtsAlt = {}
    for line in fileInput:
        f = line.split(" ")
        g = f[1].split("/")
        # OLD CP # originalFile = offDataset + '_000_%d'%index
        originalFile = 'bambu-output-file-tmp' + '_000_%d'%index
        if debug == 1:
            print ' Key: %s  Name: %s  NEvts: %d'%(originalFile,g[-1],int(f[2]))
        files[originalFile] = g[-1] 
        nevts[originalFile] = int(f[2])
        nevtsAlt[g[-1]] = int(f[2])
        index += 1
    fileInput.close()
    

# No specific task means just testing
if lRetry == 0 and lRemove == 0:
    test = 1

# Debug mode is always in test
if debug == 1:
    test = 1

# Create directories necessary to work
logDir  = catalogDir + '/condor/' + mitCfg + '/' + version + '/' + dataset
procDir = ' '
doneDir = ' '
rawDir  = ' '
if os.path.exists(logDir):
    procDir =  catalogDir + '/condor/' + mitCfg + '/' + version + '/' + dataset + '/proc'
    doneDir =  catalogDir + '/condor/' + mitCfg + '/' + version + '/' + dataset + '/done'
    rawDir  =  catalogDir + '/' + mitCfg + '/' + version + '/' + dataset
    if re.search('crab_0',rawDir):
        rawDir =  catalogDir + '/' + mitCfg + '/' + version + '/' + offDataset
    cmd     = "mkdir -p " + procDir + ' ' + doneDir + ' ' + rawDir
    status = os.system(cmd)
    if status != 0:
        print " Directory for the requested datasets could not be created. -> EXIT"
        sys.exit(1)        
else:
    print " Directory request: " + logDir
    print " Directory for the requested dataset does not exist. -> EXIT"
    sys.exit(1)

# --------------------------------------------------------------------------------------------------
# Here we start doing things
# --------------------------------------------------------------------------------------------------
# determine pid of this process
pid = str(os.getpid())

# Option '--retest': Reset the extraction for the entire dataset and exit
if retest == 1:
    print '\n Move all logs back to unprocessed area. \n'
    cmd = 'cd ' + logDir + '/done/; mv *.err ../'
    status1 = os.system(cmd)
    cmd = 'cd ' + logDir + '/done/; mv *.out ../'
    status2 = os.system(cmd)
    if status1 != 0 or status2 != 0:
        print " Could not move files to unproc status (command line, too long maybe?). -> EXIT"
        print '   logDir : ' + logDir
        print '   procDir: ' + procDir
        sys.exit(1)
    sys.exit(0)

# Option '--compact': Compactify the RawFiles.?? into one sorted file
if compact == 1:
    # Consolidate the existing RawFiles.?? into just one and exit
    print '\n Consolidating the raw files into one. \n'
    cmd  = 'cat ' + rawDir + '/RawFiles.?? | grep root | sort -u > /tmp/RawFiles.00.' + pid + ";"
    if debug == 1:
        cmd += 'cat /tmp/RawFiles.00.' + pid 
    status = os.system(cmd)
    if os.path.exists(rawDir + '/old'):
        cmd = 'rm -rf ' + rawDir + '/old'
        status = os.system(cmd)
    cmd = 'mkdir ' + rawDir + '/old; mv ' + rawDir + '/RawFiles.?? ' + rawDir + '/old'
    status = os.system(cmd)
    cmd = 'mv /tmp/RawFiles.00.' + pid + ' ' + rawDir + '/RawFiles.00'
    status = os.system(cmd)
    sys.exit(0)

# Are there any files or maybe too many?
cmd = 'cd ' + logDir + '; ls *.out *.err >& /dev/null'
status = os.system(cmd)
if status != 0:
    print " Could not find files or command line is too long maybe. -> EXIT"
    print '   logDir : ' + logDir
    print '   procDir: ' + procDir
    sys.exit(1)
    
# Safe the status (we are going from proc and later to done to have a save environment)
cmd = 'cd ' + logDir + '; mv *.err proc/'
status1 = os.system(cmd)
cmd = 'cd ' + logDir + '; mv *.out proc/'
status2 = os.system(cmd)
if status1 != 0 or status2 != 0:
    print " Could not move files to proc status (command line, too long maybe?). -> EXIT"
    print '   logDir : ' + logDir
    print '   procDir: ' + procDir
    sys.exit(1)
    
cmd = 'cd ' + procDir + '; cat *.out | grep \'  File \' | grep corrupted'
print 'Execute:  ' + cmd
lBroken = False
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    if not lBroken:
        lBroken = True
        print 'XXXX Broken files XXXX Broken files XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'
    # compactify line
    line = " ".join(str(line).split()).strip()
    f = line.split(" ");
    rm0 = "rm -f        " + f[1]
    rm1 = "stager_rm -M " + f[1]
    rm2 = "nsrm -f      " + f[1]
    hdfsFile = "/" + "/".join(f[1].split("/")[3:])
    rm3 = "glexec hdfs df -rm " + hdfsFile
    g = f[1].split("/")
    file = g[-1]
    rm4 = "rm           " + procDir + '/' + file + '.{err,out}'

    #hostname = os.environ['HOSTNAME']
    hostname = socket.gethostname()
    if re.search('cern.ch',hostname):
        machine = "fgrep cern.ch " + procDir + '/' + file + '.out | head -1 | sed "s/^/Machine: /"' 
    else:
        machine = "fgrep mit.edu " + procDir + '/' + file + '.out | head -1 | sed "s/^/Machine: /"' 
    os.system(machine)

    if   re.search("/castor/cern.ch",f[1]):
        cmd = rm1 + '; ' + rm2 + '; ' + rm4
        print '  ' + rm1 + '\n  ' + rm2 + '\n  ' + rm4
    elif re.search("cmsaf.mit.edu",f[1]) or re.search("/mnt/hadoop/cms/store",f[1]):
        cmd = rm3 + '; ' + rm4
        print '  ' + rm3 + '\n  ' + rm4
    else:
        cmd = rm0 + '; ' + rm4
        print '  ' + rm0 + '\n  ' + rm4

    if   lRetry == 1 and test == 0:
        print ' Deleting only log files'
        status = os.system(rm4)
    elif lRemove == 1 and test == 0:
        print ' Deleting entire job output'
        status = os.system(cmd)

    if debug ==1:
        print ' == DUMPING .err and .out =='
        dump = "cat " + procDir + '/' + file + '.out' 
        os.system(dump)
        dump = "cat " + procDir + '/' + file + '.err' 
        os.system(dump)
        

# Find out what the name of the raw file will be
cmd = 'ls -1 ' + rawDir + ' | grep RawFiles | tail -1'
rawFile = 'RawFiles.00'
#testFile = catalogDir + '/' + mitCfg + '/' + version + '/' + dataset + '/RawFiles.00'
testFile = rawDir + '/RawFiles.00'
if os.path.exists(testFile):
    for line in os.popen(cmd).readlines():  # run command
        rawFile = line[:-1]
        #print '  ' + rawFile
        f = line.split('.');
        number = int(f[1])
        number = number + 1
        rawFile = '%s.%02d'%(f[0],number)
print 'Raw file identified (in %s): \n     %s'%(rawDir,rawFile)

# --------------------------------------------------------------------------------------------------
#                           START -- THE KEY ACTION
# 
# Find new catalog entries and add them to the new raw file (or not for test run)
# --------------------------------------------------------------------------------------------------

# Create the moving file from this extract task
srcFile = "EMPTY.bak"
if official == 1:
    cmd = "date +Extracting_%y%m%d_%H%M%S.src"
    for line in os.popen(cmd).readlines():  # run command
        line = line[:-1]
        srcFile = line
    cmd = "rm -f " + srcFile + "; echo \"#!/bin/bash\" > " + srcFile
    os.system(cmd)


# Process all relevant files
cmd = 'cd ' + procDir + '; cat *.out | grep XX-CATALOG-XX'
print 'Execute:  ' + cmd
print '  For the catalog:'
if test == 0:
    fileOutput = open(rawDir + '/' + rawFile,'w')

for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    # compactify line
    line = " ".join(str(line).split()).strip()
    f = line.split(" ");
    f = f[2:];

    # are we dealing with an official production dataset?
    valid = 1
    if official == 1:
        if len(f) < 2:
            continue

        fullFile = f[0]

	try:
	    nProc    = int(f[1])
	except:
	    continue
	
        g        = fullFile.split("/");
        file     = g[-1]
        g        = g[:-2]
        dir      = "/".join(g)

        # determine lookup key
        if "_tmp.root" in file:
            baseFile = file.replace("_tmp","")
            nevt     = nevtsAlt[baseFile]
        else:
            g        = file.split("_");
            file     = "_".join(g[0:-2])
            baseFile = files[file]
            nevt     = nevts[file]

        if nProc == nevt:
            tier2 = move(srcFile,fullFile,dir + '/' + baseFile)
            # modify the catalog entry accordingly
            f[0] = dir + '/' + baseFile
        else:
            valid = 0
            print ' ERROR   : ' + file + '  -->  ' + baseFile + '  %d /%d' %(nProc,nevt)
            
    if valid == 1:
        line = " ".join(f)
        print '  ' + line
        if test == 0:
            fileOutput.write(line + '\n')

if test == 0:
    fileOutput.close()

# offical production and tier2?
if debug:
    print " Official %d   tier2: %d"%(official,tier2)
# Moving all files
if official == 1:
    if tier2:
        cmd = "glexec " + srcFile
        print ' CMD: ' + cmd
        status = os.system(cmd)

    # Removing the source file once we are done
    cmd = "rm " + srcFile
    print ' CMD: ' + cmd
    status = os.system(cmd)

# --------------------------------------------------------------------------------------------------
#                           END ---- THE KEY ACTION
# --------------------------------------------------------------------------------------------------

# Safe the status (from proc to done, or back to the original directory for test)
cmd = 'cd ' + procDir + '; mv *.err ../done/'
if test != 0:
    cmd = 'cd ' + procDir + '; mv *.err ../'
status1 = os.system(cmd)

cmd = 'cd ' + procDir + '; mv *.out ../done/'
if test != 0:
    cmd = 'cd ' + procDir + '; mv *.out ../'
status2 = os.system(cmd)

if status1 != 0 or status2 != 0:
    print " Could not move files from proc to done status (command line, too long maybe?). -> EXIT"
    print '   procDir: ' + procDir
    print '   doneDir: ' + doneDir
    sys.exit(1)

sys.exit(0)
