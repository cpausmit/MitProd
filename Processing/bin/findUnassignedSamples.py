#!/usr/bin/python
#---------------------------------------------------------------------------------------------------
# Find samples in the reviewd log files that are unassigned.
#
# Author: C.Paus                                                                   (August 15, 2015)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

#---------------------------------------------------------------------------------------------------
# H E L P E R
#---------------------------------------------------------------------------------------------------

def testTier2Disk(debug=0):
    # make sure we can see the Tier-2 disks: returns -1 on failure

    nFiles = -1

#---------------------------------------------------------------------------------------------------
# M A I N
#---------------------------------------------------------------------------------------------------
# Define string to explain usage of the script
usage  = "\nUsage: findSamples.py --logfile=<name>\n"
usage += "                      [ --exe ]\n"
usage += "                      [ --debug ]\n"
usage += "                      [ --help ]\n\n"

# Define the valid options which can be specified and check out the command line
valid = ['logfile=', \
         'help','exe','debug']
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# Get all parameters for the production
# -------------------------------------
# Set defaults for each option
logfile = ''
exe = False
debug = False

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--logfile":
        logfile = arg
    if opt == "--exe":
        exe = True
    if opt == "--debug":
        debug = True

# Is it a reasonable set of parameters?
if logfile == '':
    print usage
    sys.exit(0)
    

# Initial message 
print ''
print ' @-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@'
print ''
print '     R E V I E W : ' + logfile
print ''
print ' @-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@-@'
print ''

cmd = 'assignDatasetToSite.py --nCopies=2 --exec --dataset='
startPattern = 'Input: input.py'
errorPattern = 'ERROR - no sites for this data block, do not submit.'
lastDataset = ''
dataset = ''

# Access the logfile and search
input  = open(logfile,'r')
line = input.readline()
while (line != ''):
    line = line[:-1]
    #print line

    # find a potentially starting new dataset
    if startPattern in line:
        f = line.split(' ')
        f = f[5].split('=')
        dataset = f[1]

    # print the present dataset as missing
    if errorPattern in line:
        if dataset != lastDataset:
            print '%s%s'%(cmd,dataset)
            if exe:
                os.system(cmd + dataset)
        lastDataset = dataset

    line = input.readline()       # end of the while loop


input.close()

sys.exit(0)
