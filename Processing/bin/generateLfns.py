#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# This script take a RawFiles.00 that is standard in our cataloging and converts it into a lfns file
# that we use in the production. The stdout should be piped to that file.
#
# Example:
#   generateLfns.sh \
#     /home/cmsprod/catalog/t2mit/mc/arapyan/DoublyChargedHiggs/SM_vbf_h_gamma/RawFiles.00 \
#     > ~/cms/jobs/lfns/SM_vbf_h_gamma.lfns
#
#---------------------------------------------------------------------------------------------------
import getopt, sys, os, re
# Define string to explain usage of the script
usage =  "Usage: downloadSample.py --rawFile=<name>\n"

# Define the valid options which can be specified and check out the command line
valid = ['rawFile=']
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
rawFile = None

# Read new values from the command line
for opt, arg in opts:
    if opt == '--rawFile':
        rawFile = arg

if not rawFile:
    print usage
    print str(ex)
    sys.exit(1)

# go throughthe list
cmd = 'cat ' + rawFile
for line in os.popen(cmd).readlines():  # run command
    line       = line[:-1]              # strip '\n'
    line = re.sub('root://xrootd.cmsaf.mit.edu/','',line.rstrip())

    # extract file name and the number of events
    f = line.split(" ")
    file = f[0]
    nEvt = f[1]

    # create the block core info
    f = file.split('/')
    block = '/'.join(f[4:-1])

    # print a full lfn spec with fake block the file name and the number of events
    print '/' + block + '#00000000-0000-0000-0000-000000000000 ' + file  + ' ' + nEvt

# we are done
sys.exit(0)
