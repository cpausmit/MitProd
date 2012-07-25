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
# Author: C.Paus                                                                 (November 08, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string
import files
  
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage  = "\n"
usage += " Usage: generateCatalog.py --rawFile=<name>\n"
usage += "                           --nFilesPerSet=<name>\n"
usage += "                           --debug\n\n"
usage += "                           --help\n\n"

# Define the valid options which can be specified and check out the command line
valid = ['rawFiles=','nFilesPerSet=','debug','help']
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
dir          = None
debug        = int(0)
nFilesPerSet = 5

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--rawFiles":
        dir          = arg
    if opt == "--debug":
        debug        = int(1)
    if opt == "--nFilesPerSet":
        nFilesPerSet = int(arg)

# Deal with obvious problems
if dir == None:
    cmd = "--rawFile option not provided. This is required."
    raise RuntimeError, cmd
if not os.path.exists(dir):
    cmd = "Required file does not exist: " + dir
    raise RuntimeError, cmd

## Create directories necessary to work
#f       = rawFile.split('/');
#dir     = '/'.join(f[:-1])
#print " Directory: " + dir

cmd = 'cat ' + dir + '/RawFiles.?? | grep root | sort -u'
print 'Execute:  ' + cmd

iFileset    = 0
init        = False
filesOut    = open(dir + '/Files','w')
filesetsOut = open(dir + '/Filesets','w')
for line in os.popen(cmd).readlines():  # run command
    line = line[:-1]
    # print ' LINE: ' + line
    line = " ".join(str(line).split()).strip()
    f = line.split(' ')
    g = f[0].split('/')
    dataDir  = '/'.join(g[:-1])
    if not init:
        fileset = files.Fileset('0000',dataDir)
        init = True
    fileName = g[-1]
    if len(f) != 7: 
        print ' Length is not six: %d'%len(f)
        sys.exit(1)
    file = files.File(int(f[1]),int(f[2]),int(f[3]),int(f[4]),int(f[5]),int(f[6]),fileName);
    #file.show()

    if (fileset.nFiles()<nFilesPerSet):
        fileset.addFile(file)
    else:
        if debug > 0:
            print '\nFileset (debug - %d): '%debug + fileset.name
        fileset.showShort(filesetsOut,debug)
        if debug > 0:
            print 'Files contained (debug - %d):'%debug
        fileset.showShortFiles(filesOut,debug)
        iFileset = iFileset + 1
        name     = '%04d'%iFileset
        fileset.reset(name,dataDir)
        fileset.addFile(file)
        
if init:
    if debug > 0:
        print '\nFileset (debug - %d): '%debug + fileset.name
    fileset.showShort(filesetsOut,debug)
    if debug > 0:
        print 'Files contained (debug - %d):'%debug
    fileset.showShortFiles(filesOut,debug)
    iFileset = iFileset + 1

#===================================================================================================
# Now we add the additional files
if os.path.exists(dir + '/AddFiles.00'):
    cmd = 'cat ' + dir + '/AddFiles.00'
    print 'Execute:  ' + cmd
    init = False
    for line in os.popen(cmd).readlines():  # run command
        line = line[:-1]
        # print ' LINE: ' + line
        line = " ".join(str(line).split()).strip()
        f = line.split(' ')
        g = f[0].split('/')
        dataDir  = '/'.join(g[:-1])
        if not init:
            name     = '%04d'%iFileset
            #fileset  = files.Fileset(name,dataDir)
            fileset.reset(name,dataDir)
            init = True
        fileName = g[-1]
        if len(f) != 7: 
            print ' Length is not six: %d'%len(f)
            sys.exit(1)
        file = files.File(int(f[1]),int(f[2]),int(f[3]),int(f[4]),int(f[5]),int(f[6]),fileName);
        #file.show()
    
        if (fileset.nFiles()<nFilesPerSet):
            fileset.addFile(file)
        else:
            if debug > 0:
                print '\nFileset (debug - %d): '%debug + fileset.name
            fileset.showShort(filesetsOut,debug)
            if debug > 0:
                print 'Files contained (debug - %d):'%debug
            fileset.showShortFiles(filesOut,debug)
            iFileset = iFileset + 1
            name     = '%04d'%iFileset
            fileset.reset(name,dataDir)
            fileset.addFile(file)
            
    if init:
        if debug > 0:
            print '\nFileset (debug - %d): '%debug + fileset.name
        fileset.showShort(filesetsOut,debug)
        if debug > 0:
            print 'Files contained (debug - %d):'%debug
        fileset.showShortFiles(filesOut,debug)

filesOut.close()
filesetsOut.close()
