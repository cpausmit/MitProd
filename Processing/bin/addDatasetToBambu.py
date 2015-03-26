#!/usr/bin/python
#===================================================================================================
# Add a new dataset into the Bambu database.
#
# v1.0                                                                                  Sep 19, 2014
#===================================================================================================
import sys,os,subprocess,getopt,time
import MySQLdb

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

    # test whether this is a legitimate dataset by asking DAS
    cmd = 'das_client.py --format=plain --limit=0 --query="file dataset=' + \
	  dataset + ' | count(file), sum(file.size)"| sort -u'
    nFiles = ''
    size = ''
    for line in subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE).stdout.readlines():
        line    = line[:-1]
        f = line.split('=')
        if f[0] == 'count(file)':
            nFiles = f[1]
        if f[0] == 'sum(file.size)':
            size = f[1]

    if line != '':
	print '\n DAS - %s --> %s (nFiles: %s)\n'%(dataset,size,nFiles)
    else:
	print ' Error - dataset not found with das_client.py.'
	sys.exit(1)
    
#===================================================================================================
# Main starts here
#===================================================================================================
# Define string to explain usage of the script
usage =  " Usage: addDataset.py  --dataset=<name>\n"
usage += "                     [ --help ]\n\n"

# Define the valid options which can be specified and check out the command line
valid = ['dataset=','help']
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

# Read new values from the command line
for opt, arg in opts:
    if opt == "--help":
        print usage
        sys.exit(0)
    if opt == "--dataset":
        dataset  = arg

testLocalSetup(dataset,debug)

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

if len(results) > 0:
    print ' Dataset exists already in database. Do not insert again.'
    sys.exit(0)

# Prepare SQL query to INSERT a new record into the database.
sql = "insert into Datasets(DatasetProcess, DatasetSetup, DatasetTier)" + \
      " values('%s','%s','%s')"% \
        (process,setup,tier)

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
