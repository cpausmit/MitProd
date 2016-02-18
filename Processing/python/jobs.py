#---------------------------------------------------------------------------------------------------
"""

This set of classes is meant to describe the results of jobs that have run on some computing system.

"""
#---------------------------------------------------------------------------------------------------
import os, operator


#---------------------------------------------------------------------------------------------------
"""
A quasi Singleton: jobLogDescription

Full description of the log file quantities that are kept for analysis and summary.
"""
#---------------------------------------------------------------------------------------------------
def getJobLogDescription():
    "A description of the log file summary."
    patterns = { "site": "SyncSite=",
                 "host": "WNHostName=",
                 "time": "ExeTime=",
                 "exeExit": "ExeExitCode=",
                 "stageOutExit": "StageOutExitStatus = ",
                 "jobExit": "JobExitCode=" }
    return patterns

#---------------------------------------------------------------------------------------------------
"""
Class:  JobLog

Log file summary for a single job.
"""
#---------------------------------------------------------------------------------------------------
class JobLog:
    def __init__(self):
        # constructor to connect with existing setup
        self.file = ''
        self.values = {}

    def getValue(self,key):
        # return the assigned key value, or if key not available 'undefined key'
       
        value = "undefined key"
        if key in self.values:
            value = self.values[key]

        return value

    def createFromLogFile(self,file):
        # read job status from prepared summary files

        # is this an output file?
        if "stdout" not in file:
            return
    
        summaryFile = file.replace("stdout","summary")
    
        # initialization
        patterns = getJobLogDescription() 
        self.values = getJobLogDescription() 
        
        cmd = 'cat ' + file
        for line in os.popen(cmd).readlines():  # run command
            line   = line[:-1]              # strip '\n'
            
            for k,v in patterns.items():
                # pattern by pattern analysis per log file line
                if v in line:
                    value = line.replace(v,"")
                    self.values[k] = value
    
        # log this into the summary file
        output = open(summaryFile,'w')
        for k,v in self.values.items():
            output.write(' %s: %s\n'%(k,v))
        output.close()
    
        self.file = summaryFile
        if self.values["exeExit"] != "0":
            print ' ERROR in file: ' + self.file
            for k,v in sorted(self.values.items()):
                v = self.values[k]
                print ' %3d -- %s'%(v,k)

    def readFromSummaryFile(self,file):
        # read job status from prepared summary files

        # initialization
        patterns = getJobLogDescription()
        self.values = getJobLogDescription()

        # set the summary file read
        self.file = file
       
        # open the specified file
        input = open(file,'r')
        line = input.readline()

        # loop through all lines in there
        while (line != ''):
            if line[0] != '#':
                line = line[:-1]
            #print "LINE - " + line
            for k,v in patterns.items():
                if line.startswith(' '+k+': '):
                    value = line.replace(' ' + k + ': ',"")
                    #print ' Key: %s  Value: %s  --> Value: %s'%(k,v,value)
                    self.values[k] = value
            line = input.readline()

        # close the input file
        input.close()

        #print '\n Values read from file: ' + file
        #print self.values

#---------------------------------------------------------------------------------------------------
"""
Class:  JobLogsSummary

Summary of all log files of a given task for statistical analysis.
"""
#---------------------------------------------------------------------------------------------------
class JobLogsSummary:

    def __init__(self,crabId):
        # constructor to connect with existing setup
        self.crabId = crabId
        self.dataset = self.findDatasetName()
        self.jobLogs = []

    def findDatasetName(self):
        # add a given job log to our summary list
        cmd = 'grep LFNBaseName= ' + self.crabId + '/job/CMSSW.sh | cut -d/ -f5,6'
        for line in os.popen(cmd).readlines():
            line = line[:-1]
        return line

    def addJobLog(self,jobLog):
        # add a given job log to our summary list
        self.jobLogs.append(jobLog)

    def show(self):
        # present the current set of Meta Data
        print ''
        print ' CRAB ID: %s -->  %s'%(self.crabId,self.dataset)
        print ' Number of jobLogs: %d\n'%(len(self.jobLogs))

        # initialize
        nErrorCounts = {}
        tagEe = 'exeExit'
        failingSites = []

        for jobLog in self.jobLogs:
            exitCode = jobLog.getValue(tagEe)
            site = jobLog.getValue('site')

            if exitCode != '0' and site not in failingSites:
                failingSites.append(site)
            
            if exitCode in nErrorCounts:
                nErrorCounts[exitCode] += 1
            else:
                nErrorCounts[exitCode] = 1

        print '   # -- Exit Code'
        print '------------------'
        # order highest occurence first
        for k,v in reversed(sorted(nErrorCounts.items(),key=operator.itemgetter(1))):
            v = nErrorCounts[k]
            print ' %3d -- %s'%(v,k)
        sites = ''
        for site in failingSites:
            sites += ' ' + site
        print '------------------'
        print ' Failing Sites:' + sites
        print ''

        nErrorCounts = {}
        tagSe = 'stageOutExit'
        failingSites = []

        for jobLog in self.jobLogs:
            exitCode = jobLog.getValue(tagSe)
            
            site = jobLog.getValue('site')

            if exitCode != '0' and site not in failingSites:
                failingSites.append(site)

            if exitCode in nErrorCounts:
                nErrorCounts[exitCode] += 1
            else:
                nErrorCounts[exitCode] = 1

        print '   # -- Stageout Exit Code'
        print '---------------------------'
        # order highest occurence first
        for k,v in reversed(sorted(nErrorCounts.items(),key=operator.itemgetter(1))):
            v = nErrorCounts[k]
            print ' %3d -- %s'%(v,k)
        sites = ''
        for site in failingSites:
            sites += ' ' + site
        print '--------------------------'
        print ' Failing Sites:' + sites
        print ''
