#---------------------------------------------------------------------------------------------------
# Python Module File to describe Condor task
#
# Author: C.Paus                                                                      (Jun 16, 2016)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

#---------------------------------------------------------------------------------------------------
"""
Class:  Sample(dataset='undefined')
Each sample can be described through this class
"""
#---------------------------------------------------------------------------------------------------
class Sample:
    "Description of a datasample to be produced using CRAB"

    #-----------------------------------------------------------------------------------------------
    # constructor
    #-----------------------------------------------------------------------------------------------
    def __init__(self,dataset='undefined',dbs='instance=prod/global'):

        self.dataset = dataset
        self.dbs = 'instance=prod/global'
        self.nEvents = 0
        self.nEvtTotal = 0
        self.nAllLfns = 0
        self.nCompletedLfns = 0
        self.nMissingLfns = 0
        self.allLfns = {}
        self.completedLfns = {}
        self.missingLfns = {}

    #-----------------------------------------------------------------------------------------------
    # present the current samples
    #-----------------------------------------------------------------------------------------------
    def show(self):

        print ' Dataset       : ' + self.dataset
        print ' Dbs           : ' + self.dbs
        print ' NEvtTotal     : ' + str(self.nEvtTotal)
        print ' All Lfns      : ' + str(self.nAllLfns)
        print ' Completed Lfns: ' + str(self.nCompletedLfns)
        print ' Missing Lfns  : ' + str(self.nMissingLfns)

    #-----------------------------------------------------------------------------------------------
    # load all lfns relevant to this task
    #-----------------------------------------------------------------------------------------------
    def loadAllLfns(self, lfnFile):

        # initialize from scratch
        self.allLfns = {}
        self.nAllLfns = 0
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
            self.nAllLfns += 1

        print ' TOTAL   - Lfns: %6d  [ Events: %9d ]'\
              %(len(self.allLfns),self.nEvtTotal)

    #-----------------------------------------------------------------------------------------------
    # load all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def addCompletedLfn(self,file):

        if file not in self.allLfns.keys():
            print ' ERROR -- found completed lfn not in list of all lfns?! ->' + file + '<-'
        if file in self.completedLfns.keys():
            print " ERROR -- completed lfn appeared twice! This should never happen. EXIT."
            sys.exit(1)
        # add this lfn to the mix
        self.completedLfns[file] = self.allLfns[file]
        self.nCompletedLfns += 1

        return

    #-----------------------------------------------------------------------------------------------
    # load all lfns relevant to this task
    #-----------------------------------------------------------------------------------------------
    def createMissingLfns(self):

        # fill the remaining lfns from complete database
        self.nMissingLfns = 0
        self.missingLfns = {}
        for file,lfn in self.allLfns.iteritems():
            if file in self.missingLfns.keys():
                print " ERROR -- missing lfn appeared twice! This should never happen. EXIT."
                sys.exit(1)
            # is it already completed?
            if file not in self.completedLfns.keys():
                # adding this one to the missing ones
                self.missingLfns[file] = lfn
                self.nMissingLfns += 1

        print ' MISSING - Lfns: %6d'%(self.nMissingLfns)

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

        self.home = self.findHome()
        self.tag = tag
        self.submitCmd = 'submit_' +  self.tag + '.cmd'
        self.base = "/cms/store/user/paus"
        self.mitCfg = mitCfg
        self.mitVersion = mitVersion
        self.cmssw = cmssw
        self.cmsswVersion = self.findCmsswVersion()
        self.sample = Sample(dataset,dbs)
        self.sample.loadAllLfns(lfnFile)
        self.loadCompletedLfns()
        self.sample.createMissingLfns()

        self.logs = self.home + '/cms/logs/' + self.mitCfg + '/' + self.mitVersion \
            + '/' + self.sample.dataset
        self.outputData = self.home + '/cms/data/' + self.mitCfg + '/' + self.mitVersion \
            + '/' + self.sample.dataset
        self.executable = self.logs + '/makeBambu.sh'
        self.tarBall = self.logs + '/bambu_' + self.cmsswVersion + '.tgz'

        print ''
        self.show()

    #-----------------------------------------------------------------------------------------------
    # add specification to given file for exactly one more condor queue request for a given lfn
    #-----------------------------------------------------------------------------------------------
    def addLfn(self,fileH,file,lfn):

        gpack = file.replace('.root','')

        fileH.write("Arguments             = " + self.mitCfg + ' ' + self.mitVersion + ' ' \
                        + ' ' + self.cmssw + ' ' + self.sample.dataset + ' ' + gpack + ' ' \
                        + lfn + ' ' + self.tag + '\n')
        fileH.write("Output = " + self.logs + '/' + gpack + '.out' + '\n')
        fileH.write("Error = " + self.logs + '/' + gpack + '.err' + '\n')
        fileH.write("transfer_output_files = " + gpack + '.empty' + '\n')
        fileH.write("Queue" + '\n')

    #-----------------------------------------------------------------------------------------------
    # generate actual tarball, or leave as is if already up to date
    #-----------------------------------------------------------------------------------------------
    def createDirectories(self):

        # log and output data dirs
        print " INFO - make local directories "
        os.system("mkdir -p " + self.logs + " " + self.outputData)

        # remote directories for bambu output
        print " INFO - make remote directories "
        os.system("makedir " + self.base + "/" + self.mitCfg + '/' + self.mitVersion \
                      + '/' + self.sample.dataset)
        os.system("changemod --options=a+rwx " + self.base + "/" + self.mitCfg + '/' \
                      + self.mitVersion + '/' + self.sample.dataset)

    #-----------------------------------------------------------------------------------------------
    # find the present CMSSW version
    #-----------------------------------------------------------------------------------------------
    def findCmsswVersion(self):

        return (os.getenv('CMSSW_VERSION')).replace('CMSSW_','')

    #-----------------------------------------------------------------------------------------------
    # find the home directory where we submit
    #-----------------------------------------------------------------------------------------------
    def findHome(self):

        return "/home/cmsprod"

    #-----------------------------------------------------------------------------------------------
    # load all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def loadCompletedLfns(self):

        # initialize from scratch
        path = self.base + '/' + self.mitCfg + '/' + self.mitVersion + '/' \
            + self.sample.dataset 
        cmd = 'list ' + path + '  2> /dev/null | grep root'
        for line in os.popen(cmd).readlines():  # run command
            f    = line.split()
            file = (f[1].split("/")).pop()
            self.sample.addCompletedLfn(file)
        print ' DONE    - Lfns: %6d'%(self.sample.nCompletedLfns)

    #-----------------------------------------------------------------------------------------------
    # generate actual tarball, or leave as is if already up to date
    #-----------------------------------------------------------------------------------------------
    def makeTarBall(self):

        if os.path.exists(os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz"):
            print " INFO - tar ball exists: " \
                + os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz"
        else:
            print ' Make bambu tar ball: ' \
                + os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz"
            cmd = "cd " + os.getenv('CMSSW_BASE') \
                + "; tar fzc bambu_" + self.cmsswVersion + ".tgz lib/ python/ src/"
            os.system(cmd)

        cmd = "cp " + os.getenv('CMSSW_BASE') + "/bambu_" + self.cmsswVersion + ".tgz " \
                      + self.logs
        #print " COPY TARBALL: " + cmd
        os.system(cmd)
        cmd = "cp " + os.getenv('MIT_PROD_DIR') + "/bin/makeBambu.sh " + self.logs
        #print " COPY SCRIPT: " + cmd
        os.system(cmd)
            
    #-----------------------------------------------------------------------------------------------
    # present the current condor task
    #-----------------------------------------------------------------------------------------------
    def show(self):

        print ' ==== Condor Task Information (%s, %s, %s) ====' \
            %(self.tag,self.mitCfg,self.mitVersion)
        print ' '
        self.sample.show()
        print ' '

    #-----------------------------------------------------------------------------------------------
    # submit condor job
    #-----------------------------------------------------------------------------------------------
    def condorSubmit(self):

        # start with the base submit script
        cmd = 'condor_submit ' +  self.submitCmd
        os.system(cmd)

    #-----------------------------------------------------------------------------------------------
    # write condor submission configuration
    #-----------------------------------------------------------------------------------------------
    def writeCondorSubmit(self):

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
                self.addLfn(fileH,file,lfn)
