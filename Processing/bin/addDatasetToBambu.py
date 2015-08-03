#!/usr/bin/python
#===================================================================================================
# Add a new dataset into the Bambu database.
#
# v1.0                                                                                  Sep 19, 2014
#===================================================================================================
import sys,os,subprocess,getopt,time
import MySQLdb

def convertSizeToGb(sizeTxt):

    # first make sure string has proper basic format
    if len(sizeTxt) < 3:
        print ' ERROR - string for sample size (%s) not compliant. EXIT.'%(sizeTxt)
        sys.exit(1)

    if sizeTxt.isdigit(): # DAS decides to give back size in bytes
        sizeGb = int(sizeTxt)/1000./1000./1000.        
    else:              # DAS gives human readable size with unit integrated
        # this is the text including the size units, that need to be converted
        sizeGb  = float(sizeTxt[0:-2])
        units   = sizeTxt[-2:]
        # decide what to do for the given unit
        if   units == 'MB':
            sizeGb = sizeGb/1000.
        elif units == 'GB':
            pass
        elif units == 'TB':
            sizeGb = sizeGb*1000.
        else:
            print ' ERROR - Could not identify size. EXIT!'
            sys.exit(0)

    # return the size in GB as a float
    return sizeGb

def testLocalSetup(dataset,debug=0):
    # test all relevant components and exit is something is off

    # check the user proxy
    rc = os.system("voms-proxy-info -exists")
    if debug > 0:
        print 'Return code: %d'%(rc)

    if rc==0:
        if debug > 0:
            print ' User proxy is valid.'
    else:
        print ' Error - no valid proxy. EXIT!'
        sys.exit(1)

    # check the input parameters
    if dataset == '':
        print ' Error - no dataset specified. EXIT!\n'
        print usage
        sys.exit(1)

    # check basic dataset parameters
    f = dataset.split('/')
    if len(f) != 4 or f[0] != '':
        print '\n ERROR in dataset format. Please check dataset name.\n'
        print usage
        sys.exit(1)
	
    # check das_client.py tool
    cmd = 'which das_client.py'
    for line in subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE).stdout.readlines():
        line    = line[:-1]
    if line != "":
        if debug > 0:
            print ' Using das_client.py from: ' + line
    else:
        print ' Error - das_client.py in your path, please find it and add it to PATH. EXIT!'
        sys.exit(1)

def findDatasetProperties(dataset,dbs,debug=0):
    # test whether this is a legitimate dataset by asking DAS and determine size and number of files

    cmd = 'das_client.py --format=plain --limit=0 --query="file dataset=' + \
	  dataset + ' instance=' + dbs + ' | count(file), sum(file.size)"| sort -u'
    nFiles = ''
    size = ''
    units = 'GB'
    #print "CMD " + cmd
    for line in subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE).stdout.readlines():
        line    = line[:-1]
        f = line.split('=')
        if f[0] == 'count(file)':
            nFiles = f[1]
        if f[0] == 'sum(file.size)':
            size = f[1]

    if line != '':
        try:
            #print ' SIZE %s  NFILES: %s'%(size,nFiles)
                # this is the text including the size units, that need to be converted
            sizeGb = convertSizeToGb(size)
            #print ' SIZE %f'%(sizeGb)
            nFiles = int(nFiles)
            #print ' NFiles %d'%(nFiles)
        except:
            print '\n Error - could not convert size and number of files (%s %s / %s).'\
                %(size,units,nFiles)
            sys.exit(1)
        print '\n DAS - %s --> %.1f %s (nFiles: %d)\n'%(dataset,sizeGb,units,nFiles)
    else:
	print '\n Error - dataset not found with das_client.py.\n'
	sys.exit(1)

    return (sizeGb, nFiles)
    
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage =  " Usage: addDataset.py  --dataset=<name>\n"
usage += "                     [ --dbs='prod/global' ]\n"
usage += "                     [ --help ]\n\n"

# Define the valid options which can be specified and check out the command line
valid = ['dataset=','dbs=','help']
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
debug = 0
dataset  = ''
dbs = 'prod/global'

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--dataset":
        dataset = arg
    if opt == "--dbs":
        dbs = arg

testLocalSetup(dataset,debug)
(sizeGb, nFiles) = findDatasetProperties(dataset,dbs,debug)

# Decompose dataset into the three pieces (process, setup, tier)
f = dataset.split('/')
process = f[1]
setup   = f[2]
tier    = f[3]

# Open database connection
db = MySQLdb.connect(read_default_file="/etc/my.cnf",read_default_group="mysql",db="Bambu")

# Prepare a cursor object using cursor() method
cursor = db.cursor()

# First check whether this download request already exists in the database
sql = "select * from Datasets where DatasetProcess='%s' and DatasetSetup='%s' and DatasetTier='%s';"\
      %(process,setup,tier)
if debug>0:
    print ' select: ' + sql
try:
    # Execute the SQL command
    cursor.execute(sql)
    results = cursor.fetchall()
except:
    print " Error (%s): unable to fetch data."%(sql)
    sys.exit(0)

if len(results) == 1:
    print ' Dataset exists once in database. Do not insert again, but update properties.\n'
    for row in results:
        process = row[1]
        setup = row[2]
        tier = row[3]
        dbs = row[4]
        dbSizeGb = float(row[5])
        dbNFiles = int(row[6])
    # check whether information correct and adjust if needed
    if dbSizeGb != sizeGb or dbNFiles != nFiles:
        print " Update!  Size: %.3f -> %.3f  nFiles: %d -> %d"%(dbSizeGb,sizeGb,dbNFiles,nFiles)
        sql = "update Datasets set DatasetSizeGb=%f, DatasetNFiles=%d where "%(sizeGb,nFiles) + \
            " DatasetProcess='%s' and DatasetSetup='%s' and DatasetTier='%s'"%(process,setup,tier)
        print " Sql: " + sql
        try:
            # Execute the SQL command
            cursor.execute(sql)
            # Commit your changes in the database
            db.commit()
            print ' database entry was updated.'
        except:
            print ' ERROR -- insert failed, rolling back.'
            # Rollback in case there is any error
            db.rollback()
            sys.exit(1)
        # we are done here, no mmore insert
    else:
        print " Database is up to date.\n"

    sys.exit(0)

elif len(results) > 1:
    print ' Dataset exists already multiple times in database. ERROR please fix.'
    sys.exit(0)

# Prepare SQL query to INSERT a new record into the database.
sql = "insert into Datasets(" \
    + "DatasetProcess,DatasetSetup,DatasetTier,DatasetDbsInstance,DatasetSizeGb,DatasetNFiles" + \
      ") values('%s','%s','%s','%s',%f,%d)"%(process,setup,tier,dbs,sizeGb,nFiles)

if debug>0:
    print ' insert: ' + sql
try:
    # Execute the SQL command
    cursor.execute(sql)
    # Commit your changes in the database
    db.commit()
except:
    print ' ERROR -- insert failed, rolling back.'
    # Rollback in case there is any error
    db.rollback()
    sys.exit(1)
    
print ' New dataset successfully inserted into the database (%s).'%(dataset)

# disconnect from server
db.close()
