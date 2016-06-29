#---------------------------------------------------------------------------------------------------
# Python Module File to describe Condor task
#
# Author: C.Paus                                                                      (Jun 16, 2016)
#---------------------------------------------------------------------------------------------------
import os,sys,re,string,socket

#---------------------------------------------------------------------------------------------------
"""
Class:  Scheduler(host='ce04.cmsaf.mit.edu',user='paus')
Each sample can be described through this class
"""
#---------------------------------------------------------------------------------------------------
class Scheduler:
    "Description of a scheduler using condor"

    #-----------------------------------------------------------------------------------------------
    # constructor
    #-----------------------------------------------------------------------------------------------
    def __init__(self,host='ce04.cmsaf.mit.edu',user='paus'):

        self.host = host
        self.user = user
        self.here = socket.gethostname()
        self.home = self.findHome(host,user)

    #-----------------------------------------------------------------------------------------------
    # find the home directory where we submit
    #-----------------------------------------------------------------------------------------------
    def findHome(self,host,user):

        cmd = 'ssh -x ' + user + '@' + host + ' pwd'
        for line in os.popen(cmd).readlines():  # run command
            line = line[:-1]
        home = line

        return home

    #-----------------------------------------------------------------------------------------------
    # Is the scheduler local?
    #-----------------------------------------------------------------------------------------------
    def isLocal(self):

        return (self.host == self.here)

    #-----------------------------------------------------------------------------------------------
    # show the scheduler parameters
    #-----------------------------------------------------------------------------------------------
    def show(self):

        print ' ====  S c h e d u l e r  ===='
        print ' Here: ' + self.here
        print ' Host: ' + self.host
        print ' User: ' + self.user
        print ' Home: ' + self.home

#---------------------------------------------------------------------------------------------------
"""
Class:  Sample(dataset='undefined')
Each sample can be described through this class
"""
#---------------------------------------------------------------------------------------------------
class Sample:
    "Description of a datasample to be produced using condor"

    #-----------------------------------------------------------------------------------------------
    # constructor
    #-----------------------------------------------------------------------------------------------
    def __init__(self,dataset='undefined',dbs='instance=prod/global'):

        self.dataset = dataset
        self.dbs = 'instance=prod/global'
        self.nEvents = 0
        self.nEvtTotal = 0
        self.nMissingLfns = 0
        self.allLfns = {}
        self.queuedLfns = {}
        self.noCatalogLfns = {}
        self.completedLfns = {}
        self.missingLfns = {}

    #-----------------------------------------------------------------------------------------------
    # present the current samples
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print ' ====  S a m p l e  ===='
        print ' Dataset       : ' + self.dataset
        print ' Dbs           : ' + self.dbs
        print ' NEvtTotal     : ' + str(self.nEvtTotal)
        print ' All Lfns      : ' + str(len(self.allLfns))
        print ' Queued Lfns   : ' + str(len(self.queuedLfns))
        print ' NoCatalog Lfns: ' + str(len(self.noCatalogLfns))
        print ' Completed Lfns: ' + str(len(self.completedLfns))
        print ' Missing Lfns  : ' + str(len(self.missingLfns))

    #-----------------------------------------------------------------------------------------------
    # load all lfns relevant to this task
    #-----------------------------------------------------------------------------------------------
    def loadAllLfns(self, lfnFile):

        # initialize from scratch
        self.allLfns = {}
        self.nEvtTotal = 0
        # use the complete lfn file list
        cmd = 'cat ' + lfnFile
        for line in os.popen(cmd).readlines():  # run command
            line = line[:-1]
            # get ride of empty or commented lines
            if line == '' or line[0] == '#':
                continue

            # decoding the input line
            f       = line.split() # splitting every blank
            lfn     = f[1]
            file    = (f[1].split("/")).pop()

            self.nEvents = int(f[2])
            self.nEvtTotal += self.nEvents
            if file in self.allLfns.keys():
                print " ERROR -- lfn appeared twice! This should never happen. EXIT."
                sys.exit(1)
            # add this lfn to the mix
            self.allLfns[file] = lfn

        print ''
        print ' TOTAL   - Lfns: %6d  [ Events: %9d ]'\
              %(len(self.allLfns),self.nEvtTotal)

    #-----------------------------------------------------------------------------------------------
    # add all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def addQueuedLfn(self,file):

        if file not in self.allLfns.keys():
            print ' ERROR -- found queued lfn not in list of all lfns?! ->' + file + '<-'
        if file in self.queuedLfns.keys():
            print " ERROR -- queued lfn appeared twice! This should never happen. EXIT."
            sys.exit(1)
        # add this lfn to the mix
        self.queuedLfns[file] = self.allLfns[file]

        return

    #-----------------------------------------------------------------------------------------------
    # add all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def addNoCatalogLfn(self,file):

        if file not in self.allLfns.keys():
            print ' ERROR -- found queued lfn not in list of all lfns?! ->' + file + '<-'
        if file in self.noCatalogLfns.keys():
            print " ERROR -- noCatalog lfn appeared twice! This should never happen. EXIT."
            sys.exit(1)
        # add this lfn to the mix
        self.noCatalogLfns[file] = self.allLfns[file]

        return

    #-----------------------------------------------------------------------------------------------
    # add all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def addCompletedLfn(self,file):

        if file not in self.allLfns.keys():
            print ' ERROR -- found completed lfn not in list of all lfns?! ->' + file + '<-'
        if file in self.completedLfns.keys():
            print " ERROR -- completed lfn appeared twice! This should never happen. EXIT."
            sys.exit(1)
        # add this lfn to the mix
        self.completedLfns[file] = self.allLfns[file]

        return

    #-----------------------------------------------------------------------------------------------
    # load all lfns relevant to this task
    #-----------------------------------------------------------------------------------------------
    def createMissingLfns(self):

        # fill the remaining lfns from complete database
        self.missingLfns = {}
        for file,lfn in self.allLfns.iteritems():
            if file in self.missingLfns.keys():
                print " ERROR -- missing lfn appeared twice! This should never happen. EXIT."
                sys.exit(1)
            # is it already completed?
            if file not in self.completedLfns.keys() and \
               file not in self.noCatalogLfns.keys() and \
               file not in self.queuedLfns.keys():
                # adding this one to the missing ones
                self.missingLfns[file] = lfn

        print ' MISSING - Lfns: %6d'%(len(self.missingLfns))

#---------------------------------------------------------------------------------------------------
"""
Class:  CondorTask(tag,mitCfg,mitVersion,cmssw,dataset,dbs,lfnFile)
Each task in condor can be described through this class
"""
#---------------------------------------------------------------------------------------------------
class CondorTask:
    "Description of a Task in condor"

    #-----------------------------------------------------------------------------------------------
    # constructor for new creation
    #-----------------------------------------------------------------------------------------------
    def __init__(self,tag,mitCfg,mitVersion,cmssw,dataset,dbs,lfnFile):

        # fixed
        #self.scheduler = Scheduler('t3serv015.mit.edu','cmsprod')
        self.scheduler = Scheduler('ce04.cmsaf.mit.edu','paus')
        self.base = "/cms/store/user/paus"

        # from the call
        self.tag = tag
        self.mitCfg = mitCfg
        self.mitVersion = mitVersion
        self.cmssw = cmssw

        # the sample input
        self.sample = Sample(dataset,dbs)
        self.sample.loadAllLfns(lfnFile)
        self.loadQueuedLfns()
        self.loadCompletedLfns()
        self.sample.createMissingLfns()

        # derived
        self.cmsswVersion = self.findCmsswVersion()
        self.nJobs = 0
        self.submitCmd = 'submit_' +  self.tag + '.cmd'
        self.logs = self.scheduler.home + '/cms/logs/' + self.mitCfg + '/' + self.mitVersion \
            + '/' + self.sample.dataset
        self.outputData = self.scheduler.home + '/cms/data/' + self.mitCfg + '/' + self.mitVersion \
            + '/' + self.sample.dataset
        self.executable = self.logs + '/makeBambu.sh'
        self.tarBall = self.logs + '/bambu_' + self.cmsswVersion + '.tgz'

        # show what we got
        print ''
        self.show()
        print ''

    #-----------------------------------------------------------------------------------------------
    # add specification to given file for exactly one more condor queue request for a given lfn
    #-----------------------------------------------------------------------------------------------
    def addLfn(self,fileH,file,lfn):

        gpack = file.replace('.root','')

        fileH.write("Arguments = " + self.mitCfg + ' ' + self.mitVersion + ' ' \
                        + ' ' + self.cmssw + ' ' + self.sample.dataset + ' ' + gpack + ' ' \
                        + lfn + ' ' + self.tag + '\n')
        fileH.write("Output = " + self.logs + '/' + gpack + '.out' + '\n')
        fileH.write("Error = " + self.logs + '/' + gpack + '.err' + '\n')
        fileH.write("transfer_output_files = " + gpack + '.empty' + '\n')
        fileH.write("Queue" + '\n')

    #-----------------------------------------------------------------------------------------------
    # remove remainders from submission
    #-----------------------------------------------------------------------------------------------
    def cleanUp(self):

        # log and output data dirs
        print " INFO - removing submit script "
        os.system("rm -rf " + self.submitCmd)

    #-----------------------------------------------------------------------------------------------
    # submit condor job
    #-----------------------------------------------------------------------------------------------
    def condorSubmit(self):

        # make sure this condorTask has jobs to be submitted
        if self.nJobs<1:
            return

        # start with the base submit script
        cmd = 'condor_submit ' +  self.submitCmd
        if not self.scheduler.isLocal():
            cmd = 'ssh -x ' + self.scheduler.user + '@' + self.scheduler.host \
                + ' \"cd ' + self.logs + '; ' + cmd + '\"'
        os.system(cmd)

    #-----------------------------------------------------------------------------------------------
    # create the required local and remote directories
    #-----------------------------------------------------------------------------------------------
    def createDirectories(self):

        # log and output data dirs
        print " INFO - make local directories "
        cmd = "mkdir -p " + self.logs + " " + self.outputData
        if not self.scheduler.isLocal():
            cmd = 'ssh -x ' + self.scheduler.user + '@' + self.scheduler.host + ' ' + cmd
        #print ' CMD: ' + cmd
        os.system(cmd)

        # remote directories for bambu output
        print " INFO - make remote directories "
        os.system("makedir --p " + self.base + "/" + self.mitCfg + '/' + self.mitVersion \
                      + '/' + self.sample.dataset)
        os.system("changemod --options=a+rwx " + self.base + "/" + self.mitCfg + '/' \
                      + self.mitVersion + '/' + self.sample.dataset)

    #-----------------------------------------------------------------------------------------------
    # find the present CMSSW version
    #-----------------------------------------------------------------------------------------------
    def findCmsswVersion(self):

        return (os.getenv('CMSSW_VERSION')).replace('CMSSW_','')

    #-----------------------------------------------------------------------------------------------
    # load all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def loadCompletedLfns(self):

        # initialize from scratch
        path = self.base + '/' + self.mitCfg + '/' + self.mitVersion + '/' \
            + self.sample.dataset 
        # first fully checked files
        cmd = 'list ' + path + '  2> /dev/null | grep root'
        for line in os.popen(cmd).readlines():  # run command
            f    = line.split()
            file = (f[1].split("/")).pop()
            self.sample.addCompletedLfn(file)
        # now also look at the temporary files (not yet cataloged)
        cmd = 'list ' + path + '/crab_*/  2> /dev/null | grep _tmp.root'
        #print " TMP files: " + cmd
        for line in os.popen(cmd).readlines():  # run command
            f    = line.split()
            file = (f[1].split("/")).pop()
            file = file.replace('_tmp','')
            self.sample.addNoCatalogLfn(file)
        print ' NOCATAL - Lfns: %6d'%(len(self.sample.noCatalogLfns))
        print ' DONE    - Lfns: %6d'%(len(self.sample.completedLfns))

    #-----------------------------------------------------------------------------------------------
    # load all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def loadQueuedLfns(self):

        # initialize from scratch
        path = self.base + '/' + self.mitCfg + '/' + self.mitVersion + '/' \
            + self.sample.dataset 
        pattern = "%s %s %s %s"%(self.mitCfg,self.mitVersion,self.cmssw,self.sample.dataset)
        cmd = 'condor_q -format \'%s\n\' Args 2> /dev/null | grep \'' + pattern + '\''
        if not self.scheduler.isLocal():
            cmd = 'ssh -x ' + self.scheduler.user + '@' + self.scheduler.host \
                + ' \"' + cmd + '\"'
        for line in os.popen(cmd).readlines():  # run command
            f    = line.split(' ')
            file = f[4] + '.root'
            self.sample.addQueuedLfn(file)
        print ' QUEUED  - Lfns: %6d'%(len(self.sample.queuedLfns))

    #-----------------------------------------------------------------------------------------------
    # generate actual tarball, or leave as is if already up to date
    #-----------------------------------------------------------------------------------------------
    def makeTarBall(self):

        # check if the tar bacll exists locally
        if os.path.exists(os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz"):
            print " INFO - tar ball exists: " \
                + os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz"
        else:
            print ' Make bambu tar ball: ' \
                + os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz"
            cmd = "cd " + os.getenv('CMSSW_BASE') \
                + "; tar fzc bambu_" + self.cmsswVersion + ".tgz lib/ python/ src/"
            os.system(cmd)

        # see whether the tar ball needs to be copied locally or to remote scheduler
        if self.scheduler.isLocal():
            cmd = "cp " + os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz " \
                + self.logs
            os.system(cmd)
            # also copy the script over
            cmd = "cp " + os.getenv('MIT_PROD_DIR') + "/bin/makeBambu.sh " + self.logs
            os.system(cmd)
        else:
            cmd = "scp -q " + os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz " \
                + self.scheduler.user + '@' +  self.scheduler.host + ':' + self.logs
            os.system(cmd)
            cmd = "scp -q " + os.getenv('MIT_PROD_DIR') + "/bin/makeBambu.sh "  \
                + self.scheduler.user + '@' +  self.scheduler.host + ':' + self.logs
            os.system(cmd)

    #-----------------------------------------------------------------------------------------------
    # present the current condor task
    #-----------------------------------------------------------------------------------------------
    def show(self):

        print ' ==== C o n d o r  T a s k  I n f o r m a t i o n  ===='
        print ' '
        print ' Tag     : ' + self.tag
        print ' Base    : ' + self.base
        print ' Config  : ' + self.mitCfg
        print ' Version : ' + self.mitVersion
        print ' CMSSW py: ' + self.cmssw
        print ' '
        self.sample.show()
        print ' '
        self.scheduler.show()

    #-----------------------------------------------------------------------------------------------
    # write condor submission configuration
    #-----------------------------------------------------------------------------------------------
    def writeCondorSubmit(self):

        # make sure to keep track of the number of jobs created
        self.nJobs = 0

        # start with the base submit script
        cmd = 'cp ' + os.getenv('MIT_PROD_DIR') + '/condor/base.sub ' +  self.submitCmd
        os.system(cmd)

        # attach the additional processing lines defining the specifc LFN productions
        with open(self.submitCmd,'a') as fileH:
            fileH.write("Environment = \"HOSTNAME=" + os.getenv('HOSTNAME') + "\"" + '\n')
            fileH.write("Initialdir = " + self.outputData + '\n')
            fileH.write("Executable = " + self.executable + '\n')
            fileH.write("Log = " + self.logs + '/' + self.sample.dataset + '.log' + '\n')
            fileH.write("transfer_input_files = " + self.tarBall + '\n')
            for file,lfn in self.sample.missingLfns.iteritems():
                print ' Adding : %s %s'%(file,lfn)
                self.nJobs += 1
                self.addLfn(fileH,file,lfn)

        # make sure submit script is in the right place
        if not self.scheduler.isLocal() and self.nJobs>0:
            cmd = "scp -q " + self.submitCmd + " " \
                + self.scheduler.user + '@' +  self.scheduler.host + ':' + self.logs
            os.system(cmd)
            cmd = "ssh -x " + self.scheduler.user + '@' +  self.scheduler.host \
                + ' \"voms-proxy-init --valid 168:00 -voms cms >& /dev/null\" '
            os.system(cmd)
