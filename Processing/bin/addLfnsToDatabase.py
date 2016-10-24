#!/usr/bin/python
#---------------------------------------------------------------------------------------------------
# Add all lfns of a given dataset to the database.
#
# v1.0                                                                                  Oct 14, 2016
#---------------------------------------------------------------------------------------------------

import sys,os,subprocess,getopt,time
import MySQLdb

def isValidId(id):
    # make sure the Id is larger zero

    valid = True
    if id<=0:
        valid = False

    return valid

def testLocalSetup(dataset):
    # test all relevant components and exit is something is off

    # check the input parameters
    if dataset == '':
        print ' Error - no dataset specified. EXIT!\n'
        print usage
        sys.exit(1)

def getBlockId(datasetId,blockName):

    blockId = -1

    sql = "select BlockId from Blocks where " \
        + "DatasetId=%d and BlockName='%s';"%(datasetId,blockName)
    try:
        # Execute the SQL command
        cursor.execute(sql)
        results = cursor.fetchall()
    except:
        print " ERROR (%s): unable to fetch data."%(sql)
        sys.exit(0)

    blockId = int(results[0][0])
    if not isValidId(blockId):
        print ' ERROR -- invalid block id: %d'%(blockId)
        sys.exit(1)
 
    return blockId

def getDatasetId(dataset):

    # Decompose dataset into the three pieces (process, setup, tier)
    f = dataset.split('+')
    process = f[0]
    setup   = f[1]
    tier    = f[2]

    sql = "select DatasetId from Datasets where " \
        + "DatasetProcess='%s' and DatasetSetup='%s' and DatasetTier='%s';"%(process,setup,tier)
    try:
        # Execute the SQL command
        cursor.execute(sql)
        results = cursor.fetchall()
    except:
        print " Error (%s): unable to fetch data."%(sql)
        sys.exit(0)
    
    if len(results) != 1:
        if delete<1:
            print ' Requested dataset not well defined, check database (nEntries=%d).'%(len(results))
        else:
            print ' Dataset not in database, deletion not needed.'
        sys.exit(0)
    else:
        datasetId = int(results[0][0])
 
    if not isValidId(datasetId):
        print ' ERROR -- invalid dataset id: %d'%(datasetId)
        sys.exit(1)
            
    return datasetId

def addBlock(datasetId,blockName):

    sql  = "insert into Blocks(DatasetId,BlockName) values(%d,'%s')"%(datasetId,blockName)
    #print " Adding block: " + sql
    try:
        # Execute the SQL command
        cursor.execute(sql)
    except:
        print ' ERROR (%s) - could not insert new block.'%(sql)
        print " Unexpected error:", sys.exc_info()[0]

    return getBlockId(datasetId,blockName)

def addLfn(datasetId,blockId,fileName,pathName,nEvents):

    sql = "insert into Lfns(DatasetId,BlockId,FileName,PathName,NEvents) " \
        +  " values(%d,%d,'%s','%s',%d)"%(datasetId,blockId,fileName,pathName,nEvents)
    #print " Adding lfn: " + sql
    try:
        # Execute the SQL command
        cursor.execute(sql)
    except:
        print ' ERROR (%s) - could not insert new file.'%(sql)
        print " Unexpected error:", sys.exc_info()[0]
    
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage  = "\n"
usage += " Usage: addLfnsToDatabase.py  --dataset=<name>\n\n"

# Define the valid options which can be specified and check out the command line
valid = ['dataset=',"help"]
try:
    opts, args = getopt.getopt(sys.argv[1:], "", valid)
except getopt.GetoptError, ex:
    print usage
    print str(ex)
    sys.exit(1)

# --------------------------------------------------------------------------------------------------
# Get all parameters for the production
# --------------------------------------------------------------------------------------------------
# Set defaults for each command line parameter/option
dataset = ''

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--dataset":
        dataset = arg
        if dataset[0] == '/':
            dataset = dataset[1:].replace('/','+')

# Make sure our local setup is good
testLocalSetup(dataset)

# Open database connection
db = MySQLdb.connect(read_default_file="/etc/my.cnf",read_default_group="mysql",db="Bambu")
cursor = db.cursor()

# First get the dataset id
datasetId = getDatasetId(dataset)

# Read the lfn file
blockId = -1
lastBlockName = "EMPTY"
with open('/home/cmsprod/cms/jobs/lfns/' + dataset + '.lfns','r') as fHandle:
    for line in fHandle:
        f = line[:-1].split(" ")
        if len(f) != 3:
            print ' ERROR invalid line: ' + line
        else:
            # Decode the relevant information
            blockName = (f[0].split("#"))[1]
            file = f[1]
            fileName = (file.split('/')).pop()
            fileName = fileName.replace('.root','')
            pathName = '/'.join((file.split('/'))[:-1])
            nEvents = int(f[2])

            if blockName != lastBlockName:
                blockId = addBlock(datasetId,blockName)

            if isValidId(blockId):
                addLfn(datasetId,blockId,fileName,pathName,nEvents)

            lastBlockName = blockName

# disconnect from server
db.close()
