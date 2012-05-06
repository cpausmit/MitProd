#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Simple interface to command line DBS to prepare my crabTask input files.
#---------------------------------------------------------------------------------------------------
import os,sys,types,string,re,getopt

# Define string to explain usage of the script
usage =  "Usage: input.py --dataset=<name>\n"
usage += "                --option=[ lfn, xml ]\n"
usage += "                [ --dbs= ]\n"
usage += "                --help\n"

def printLine(option,nEvents,block,lfn,iJob):
    if option == 'xml':
        print '  <Job MaxEvents="%d'%nEvents + '"  InputFiles="' + lfn \
              + '" SkipEvents="0" JobID="%d'%iJob + '" > </Job>'
    else:
        print "%s %s %d"%(block,lfn,nEvents)

    
# Define the valid options which can be specified and check out the command line
valid = ['db=','dbs=','dataset=','option=','help']
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
db      = None
dbs     = None
dataset = None
option  = 'lfn'

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--db":
        db      = arg
    if opt == "--dbs":
        dbs     = arg
    if opt == "--dataset":
        dataset = arg
    if opt == "--option":
        option  = arg

# Deal with obvious problems
if dataset == None:
    cmd = "--dataset=  required parameter not provided."
    raise RuntimeError, cmd

#---------------------------------------------------------------------------------------------------
# main
#---------------------------------------------------------------------------------------------------
if not db:
    # find relevant blocks
    if   dbs == 'none':
        cmd = 'dascli.py --query="block=' + dataset + '*" --limit=999999 --format=blocks'
    elif dbs == '':
        cmd = 'dbs search --query=\"find block where dataset=*' + dataset + '\"'
    elif re.search('http://',dbs):
        cmd = 'dbs search --url=' + dbs + ' --query="find block where dataset=*' + dataset + '"'
    else:
        cmd = 'echo ' + dataset + '#00000000-0000-0000-0000-000000000000'

    #print "CMD " + cmd
    cmd += "| grep \# | sort"
    # never print #print "cmd: " + cmd

    blocks = []
    iJob = 1
    if option == 'xml':
        print '<arguments>'
    for line in os.popen(cmd).readlines():
        line = line[:-1]
        blocks.append(line)
    for block in blocks:
        #print ' BLOCK: ' + block

        if   dbs == 'none':
            cmd = 'dascli.py --query="file block=' + block + '" --limit=999999 --format=files'
        elif dbs == '':
            cmd = 'dbs search --query="find file,file.numevents where block=' + block + '"'
        elif re.search('http://',dbs):
            cmd = 'dbs search --url=' + dbs + \
                  ' --query="find file,file.numevents where block=' + block + '"'
        else:
            cmd = 'cat /home/cmsprod/catalog/t2mit/private/' + dbs + dataset \
                  + '/Files | sed \'s@XX-CATALOG-XX@@\' | sed \'s@root://xrootd1.cmsaf.mit.edu/@@\''
            
        #print "CMD " + cmd
        cmd += "| grep store | sort"
        for line in os.popen(cmd).readlines():
            #print "LINE >" + line
            line = line[:-1]
            f       = line.split()
            lfn     = f[0]
            nEvents = int(f[1])
            f       = lfn.split("/")
            file    = f[-1]
            if nEvents != 0:
                printLine(option,nEvents,block,lfn,iJob)
                iJob = iJob + 1
    if option == 'xml':
        print '</arguments>'

if db:
    cmd = 'cat ' + db

    iJob = 1
    if option == 'xml':
        print '<arguments>'
    for line in os.popen(cmd).readlines():
        line    = line[:-1]
    
        f       = line.split()
        block   = f[0]
        lfn     = f[1]
        nEvents = int(f[2])
    
        f       = lfn.split("/")
        file    = f[-1]

        if nEvents != 0:
            printLine(option,nEvents,block,lfn,iJob)
            iJob = iJob + 1
    
    if option == 'xml':
        print '</arguments>'
