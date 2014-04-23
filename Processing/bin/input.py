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

def printHeader(option):
    if option == 'xml':
        print '<arguments>'
        
def printFooter(option):
    if option == 'xml':
        print '</arguments>'

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
private = False

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

# is it a private production
f = dataset.split('/')
#print " Dataset: " + dataset
if f[1] == "mc":
    private = True
    #print ' Attention -- private dataset identified.'

#---------------------------------------------------------------------------------------------------
# main
#---------------------------------------------------------------------------------------------------
# option one: use our cached version of the database
if db:
    cmd = 'cat ' + db

    iJob = 1

    printHeader(option)
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
    printFooter(option)

    sys.exit(0)

# option one: we are using a privatly produced dataset
elif private:
    lfn = '/store/user/paus' + dataset
    dir = '/mnt/hadoop/cms/store/user/paus' + dataset
    cmd = 'list ' + dir
    for line in os.popen(cmd).readlines():
        line = line[:-1]
        f = line.split(' ')
        size = int(f[0])
        file = f[1]

        cmdCount = 'catalogFile.sh /mnt/hadoop/cms' + lfn + \
                   '/' + file + ' >& /tmp/cata.TMP; grep XX-CATALOG-XX /tmp/cata.TMP' 
        #print ' COUNT: ' + cmdCount
        nEvts = 0
        for tmp in os.popen(cmdCount).readlines():
            tmp = tmp[:-1]
            #print " Check: " + tmp
            f = tmp.split(" ")
            if len(f) > 4:
                nEvts = f[4]

        if nEvts == 0:
            #os.system("cat /tmp/cata.TMP");
            for tmp in os.popen(cmdCount).readlines():
                tmp = tmp[:-1]
                #print " Check: " + tmp
                f = tmp.split(" ")
                if len(f) > 4:
                    nEvts = f[4]
            
        if nEvts > 0:
            print '%s#00000000-0000-0000-0000-000000000000 %s/%s %s'%(dataset,lfn,file,nEvts)
        #print ''
    sys.exit()

# option two: 
elif not db:

#    cmd = 'das_client.py --format=plain --limit=0 --query="file dataset=' + \
#          dataset + ' | grep file.block_name, file.name, file.nevents" | grep store | sort'
    cmd = 'das_client.py --format=plain --limit=0 --query="block dataset=' + \
          dataset + ' | grep block.name" | sort'
    blocks = []
    for line in os.popen(cmd).readlines():
        line  = line[:-1]
        blocks.append(line)

    # header if needed
    printHeader(option)

    for block in blocks:
        cmd = 'das_client.py --format=plain --limit=0 --query="file block=' + \
              block + ' | grep file.name, file.nevents" | grep store | sort'

        iJob = 1
        for line in os.popen(cmd).readlines():
            line = line[:-1]

            f       = line.split()
            lfn     = f[0]
            nEvents = int(f[1])
            
            f       = lfn.split("/")
            file    = f[-1]
            
            if nEvents != 0:
                printLine(option,nEvents,block,lfn,iJob)
                iJob = iJob + 1

    # footer if needed
    printFooter(option)

## 
##     # find relevant blocks
##     if   dbs == 'none':
##         cmd = 'dascli.py --query="block=' + dataset + '*" --limit=999999 --format=blocks'
##     elif dbs == '':
##         cmd = 'dbs search --query=\"find block where dataset=*' + dataset + '\"'
##     elif re.search('http://',dbs):
##         cmd = 'dbs search --url=' + dbs + ' --query="find block where dataset=*' + dataset + '"'
##     else:
##         cmd = 'echo ' + dataset + '#00000000-0000-0000-0000-000000000000'
## 
##     print "CMD " + cmd
##     cmd += "| grep \# | sort"
##     # never print #print "cmd: " + cmd
## 
##     blocks = []
##     iJob = 1
##     printHeader(option)
## 
##     # find all blocks
##     for line in os.popen(cmd).readlines():
##         line = line[:-1]
##         blocks.append(line)
## 
##     # find all files per block
##     for block in blocks:
##         #print ' BLOCK: ' + block
## 
##         if   dbs == 'none':
##             cmd = 'dascli.py --query="file block=' + block + '" --limit=999999 --format=files'
##         elif dbs == '':
##             cmd = 'dbs search --query="find file,file.numevents where block=' + block + '"'
##         elif re.search('http://',dbs):
##             cmd = 'dbs search --url=' + dbs + \
##                   ' --query="find file,file.numevents where block=' + block + '"'
##         else:
##             cmd = 'cat /home/cmsprod/catalog/t2mit/private/' + dbs + dataset \
##                   + '/Files | sed \'s@XX-CATALOG-XX@@\' | sed \'s@root://xrootd.cmsaf.mit.edu/@@\''
##             
##         #print "CMD " + cmd
##         cmd += "| grep store | sort"
##         for line in os.popen(cmd).readlines():
##             #print "LINE >" + line
##             line = line[:-1]
##             f       = line.split()
##             lfn     = f[0]
##             nEvents = int(f[1])
##             f       = lfn.split("/")
##             file    = f[-1]
##             if nEvents != 0:
##                 printLine(option,nEvents,block,lfn,iJob)
##                 iJob = iJob + 1
##     printFooter(option)
