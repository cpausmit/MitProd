#---------------------------------------------------------------------------------------------------
# Python Module File to describe CRAB tasks and the corresponding job stati
#
# Author: C.Paus                                                                      (Oct 10, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

#---------------------------------------------------------------------------------------------------
"""
Class:  SubTask(tag,)
Each SubTask in CRAB can be described through this class
"""
#---------------------------------------------------------------------------------------------------
class SubTask:
    "Description of a SubTask in CRAB"
    # variable to be determined
    index       = -1
    lfnFile     = 'undefined' # file containing the LFNs to be processed
    nSubTaskLfn = -1          # number of LFNs in this subtask
    #-----------------------------------------------------------------------------------------------
    # constructor to connect with existing setup
    #-----------------------------------------------------------------------------------------------
    def __init__(self,index,lfnFile):
        self.index       = index
        self.lfnFile     = lfnFile
        self.nSubTaskLfn = 0

    #-----------------------------------------------------------------------------------------------
    # present the current crab subtask
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print ' SubTask (Idx: %d,  LfnFile: %s) ===='%(self.index,self.lfnFile)

    #-----------------------------------------------------------------------------------------------
    # subtask tag
    #-----------------------------------------------------------------------------------------------
    def tag(self):
        return "%04d"%self.index

#---------------------------------------------------------------------------------------------------
"""
Class:  Task(tag,cmsDataset='undefined',mitCfg='undefined',mitVersion='undefined')
Each task in CRAB can be described through this class
"""
#---------------------------------------------------------------------------------------------------
class Task:
    "Description of a Task in CRAB"
    # this is sufficient to do anything
    tag            = 'undefined'
    # from actual crab configuration directly
    storageEle     = 'undefined'
    storagePath    = 'undefined'
    cmsDataset     = 'undefined'
    nEvents        = -1
    nTotalEvts     = -1
    # MIT specific stuff
    mitCfg         = 'undefined'
    mitVersion     = 'undefined'
    mitDataset     = 'undefined'
    localPath      = 'undefined'
    # status of task as a whole and each individual job
    status         = 'undefined'           # 'undefined', ....
    #
    # 'undefined': initial status, not yet even checked
    # 'cataloged': all jobs of the tasks have completed and are cataloged successfully
    # 'completed': all jobs of the tasks have completed successfully
    # 'finished' : all jobs have run, but unsubmitted jobs, errors or aborts might have occured
    # 'active'   : some jobs are either in Done, Running or to be Run
    #
    jobStati       = []
    failingSites   = {}
    lfns           = {}
    blocks         = {}
    # subtasks
    nSubTaskLfnMax = 400     # maximum number of LFNs in a subtask
    subTasks       = []      # list of subtasks
    #-----------------------------------------------------------------------------------------------
    # constructor to connect with existing setup
    #-----------------------------------------------------------------------------------------------
    def __init__(self,tag,cmsDataset='undefined',mitCfg='undefined',mitVersion='undefined'):
        self.tag = tag
        self.status = 'undefined'
        if tag == 'new':
            self.new(cmsDataset,mitCfg,mitVersion)
        elif not os.path.exists(tag):
            self.new(cmsDataset,mitCfg,mitVersion)
            if os.path.exists('crab.cfg'):
                print ' Loading configuration from local crab.cfg'
                cmd = 'cat crab.cfg | grep ^dataset| cut -d= -f2| tr -d \' \''
                for line in os.popen(cmd).readlines():  # run command
                    self.cmsDataset  = line[:-1]        # strip '\n'
                cmd = 'cat crab.cfg | grep ^storage_element| cut -d= -f2| tr -d \' \''
                for line in os.popen(cmd).readlines():  # run command
                    self.storageEle  = line[:-1]        # strip '\n'
                cmd = 'cat crab.cfg | grep ^storage_path| cut -d= -f2-3| tr -d \' \''
                for line in os.popen(cmd).readlines():  # run command
                    self.storagePath = line[:-1]        # strip '\n'
                cmd = 'cat crab.cfg | grep ^user_remote_dir| cut -d= -f2-3| tr -d \' \''
                for line in os.popen(cmd).readlines():  # run command
                    self.storagePath += line[:-1]       # strip '\n'
                f = (self.storagePath).split('/')
                if re.search('crab_0_',f[-1]):
                    self.mitDataset = f[-2]
                    self.mitVersion = f[-3]
                    self.mitCfg     = f[-4]
                else:
                    self.mitDataset = f[-1]
                    self.mitVersion = f[-2]
                    self.mitCfg     = f[-3]
        else:
            cmd = 'cat ' + tag + '/share/crab.cfg | grep ^dataset| cut -d= -f2| tr -d \' \''
            for line in os.popen(cmd).readlines():  # run command
                self.cmsDataset  = line[:-1]        # strip '\n'
            cmd = 'cat ' + tag + '/share/crab.cfg | grep ^storage_element| cut -d= -f2| tr -d \' \''
            for line in os.popen(cmd).readlines():  # run command
                self.storageEle  = line[:-1]        # strip '\n'
            cmd = 'cat ' + tag + '/share/crab.cfg | grep ^storage_path| cut -d= -f2-3| tr -d \' \''
            for line in os.popen(cmd).readlines():  # run command
                self.storagePath = line[:-1]        # strip '\n'
            cmd = 'cat '+tag+'/share/crab.cfg | grep ^user_remote_dir| cut -d= -f2-3| tr -d \' \''
            for line in os.popen(cmd).readlines():  # run command
                self.storagePath += line[:-1]       # strip '\n'
            f = (self.storagePath).split('/')
            if re.search('crab_0_',f[-1]):
                self.mitDataset = f[-2]
                self.mitVersion = f[-3]
                self.mitCfg     = f[-4]
            else:
                self.mitDataset = f[-1]
                self.mitVersion = f[-2]
                self.mitCfg     = f[-3]

    #-----------------------------------------------------------------------------------------------
    # constructor for new creation
    #-----------------------------------------------------------------------------------------------
    def new(self,cmsDataset,mitCfg,mitVersion):
        self.cmsDataset = cmsDataset
        self.mitCfg     = mitCfg
        self.mitVersion = mitVersion
        self.status     = 'undefined'
        # derive the missing parameters
        cmsswCfg = 'cmssw.cfg'
        crabFile  = mitCfg + '/' + mitVersion + '/' + 'crab.cfg'
        if not os.path.exists(crabFile):
            cmd = "Crab file not found: %s" % crabFile
            raise RuntimeError, cmd
        cmsswFile = mitCfg + '/' + mitVersion + '/' + cmsswCfg
        if not os.path.exists(cmsswFile):
            cmd = "Cmssw file not found: %s" % cmsswFile
            cmsswCfg = 'cmssw.py'
            cmsswFile = mitCfg + '/' + mitVersion + '/' + cmsswCfg
            if not os.path.exists(cmsswFile):
                cmd = "Cmssw file not found: %s" % cmsswFile
                cmd = " XXXX ERROR no valid configuration found XXXX"
                raise RuntimeError, cmd
        # resolve the other mitCfg parameters from the configuration file
        cmd = 'cat ' + mitCfg + '/' + mitVersion + '/' + 'Productions'
        join       = 0
        mitDataset = ""
        fullLine   = ""
        bSlash     = "\\";
        for line in os.popen(cmd).readlines():  # run command
            line = line[:-1]
            # get ride of empty or commented lines
            if line == '' or line[0] == '#':
                continue
            # join lines
            if join == 1:
                fullLine += line
            else:
                fullLine  = line
            # determine if finished or more is coming
            if fullLine[-1] == bSlash:
                join = 1
                fullLine = fullLine[:-1]
            else:
                join = 0
                # test whether there is a directory   
                names      = fullLine.split()       # splitting every blank
                if names[0] == self.cmsDataset:
                    self.mitDataset = names[1]      # the equivalent MIT name of the dataset
                    self.nEvents    = int(names[2]) # number of events to be used in the production
                    if names[4] != "-":
                        self.localPath  = names[4]
                    print "\n Sample Info: " + fullLine + "\n"
        # find the forseen storage place
        cmd = 'grep ^storage_element ' + crabFile
        for file in os.popen(cmd).readlines():   # run command
            line        = file[:-1]              # strip '\n'
            # decode the storage element name
            names       = line.split("=")        # splitting every '='
            self.storageEle  = names.pop()
            self.storageEle  = re.sub("\s", "",self.storageEle)
        cmd = 'grep ^storage_path ' + crabFile
        for line in os.popen(cmd).readlines():   # run command
            line        = line[:-1]              # strip '\n'
            # decode the storage directory name
            names       = line.split("=")        # splitting every '='
            names       = names[1:]
            self.storagePath = "=".join(names)
            names       = self.storagePath.split('/')
            names.pop();               names.pop();                   names.pop();
            names.append(self.mitCfg); names.append(self.mitVersion); names.append(self.mitDataset)
            self.storagePath = '/'.join(names)
            self.storagePath = re.sub("\s","",self.storagePath)
        cmd = 'grep ^user_remote_dir ' + crabFile
        for line in os.popen(cmd).readlines():   # run command
            line        = line[:-1]              # strip '\n'
            # decode the storage directory name
            names        = line.split(" ")       # splitting every ' '
            self.storagePath += names[-1]

    #-----------------------------------------------------------------------------------------------
    # present the current crab task
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print ' ==== CRAB Task Information (%s, %s, %s) ===='%(self.tag,self.mitCfg,self.mitVersion)
        print ' Dataset: ' + self.cmsDataset + ' (' + self.mitDataset + ')'
        print ' Storage: %s  @  %s'%(self.storagePath,self.storageEle)
        print ' List of sub tasks to be completed: '
        for subTask in self.subTasks:
            subTask.show()
        print ' '

    #-----------------------------------------------------------------------------------------------
    # create all subtasks of the tasks
    #-----------------------------------------------------------------------------------------------
    def createSubTasks(self,lfnFile):
        print ' creating subtasks'
        # loop through the missing lfn file and create subtasks each nSubTaskEvents
        cmd = 'cat ' + lfnFile
        iLine = 0
        index = 0
        output = open("/tmp/tmp.bak",'w')
        for line in os.popen(cmd).readlines():  # run command
            iLine += 1
            # open file as needed
            if iLine % self.nSubTaskLfnMax == 1:
                if output:
                    output.close()
                index       += 1
                file = lfnFile + '_%04d' % index
                output = open(file,'w')
                subTask = SubTask(index,file)
                # add this subtaks to the list
                self.subTasks.append(subTask)
            # one more lfn entry for this sub task
            output.write(line)
            subTask.nSubTaskLfn += 1           

        # closeup the last subtask
        output.close()

        print ' '
        self.show()

    #-----------------------------------------------------------------------------------------------
    # load all lfns relevant to this task
    #-----------------------------------------------------------------------------------------------
    def loadAllLfns(self, lfnFile):
        # initialize from scratch
        self.lfns       = {}
        self.blocks     = {}
        self.nTotalEvts = 0
        # use the complete lfn file list
        cmd = 'cat ' + lfnFile
        for line in os.popen(cmd).readlines():  # run command
            line = line[:-1]
            # get ride of empty or commented lines
            if line == '' or line[0] == '#':
                continue

            # decoding the input line
            f       = line.split() # splitting every blank
            block   = f[0]
            file    = f[1]
            nEvents = int(f[2])
            self.nTotalEvts += nEvents

            f       = file.split('/')
            file    = f[-1]

            if not self.lfns.get(file):
                self.lfns[file] = 0
            else:
                self.lfns[file] = 1

            if not self.blocks.get(block):
                self.blocks[block]  = 1
            else:
                self.blocks[block] += 1

        print ' TOTAL   - Lfns: %6d  [ Blocks: %4d  Events: %9d ]'\
              %(len(self.lfns),len(self.blocks),self.nTotalEvts)

    #-----------------------------------------------------------------------------------------------
    # load all lfns so far completed relevant to this task
    #-----------------------------------------------------------------------------------------------
    def loadCompletedLfns(self):
        # initialize from scratch
        self.nLfnDone = 0
        # find all already existing files
        f = self.storagePath.split('=')
        path = f[-1]
        if re.search('crab_0_',path) or  re.search('CRAB',path):
            f    = path.split('/')
            f    = f[:-1]
            path = '/'.join(f)
        cmd = 'list ' + path + ' | grep root 2> /dev/null'
        for line in os.popen(cmd).readlines():  # run command
            f    = line.split()
            file = f[1]
            if not self.lfns.get(file):
                self.lfns[file] = 2
                print ' ERROR -- found completed lfn not in list of all lfns?! -> ' + file
            else:
                self.lfns[file] = 1
            self.nLfnDone += 1
        print ' DONE    - Lfns: %6d'%(self.nLfnDone)

    #-----------------------------------------------------------------------------------------------
    # load all lfns relevant to this task
    #-----------------------------------------------------------------------------------------------
    def createMissingLfns(self, lfnFile, restLfnFile = "remaining.lfns"):
        # fill the remaining lfns from complete database
        self.status = 'cataloged'
        self.nLfnMissing = 0
        cmd = 'rm -rf ' + restLfnFile + '; touch ' + restLfnFile
        os.system(cmd)
        for lfn,status in self.lfns.iteritems():
            if   status == 0:
                # add this lfn to the file
                self.nLfnMissing += 1
                cmd = 'grep ' + lfn + ' ' + lfnFile + ' >> ' + restLfnFile
                #print ' CMD ' + cmd
                os.system(cmd)
            else:
                self.status = 'undefined'

        # it is important to sort them (by first column == block)
        cmd = 'sort -u ' + restLfnFile + ' > /tmp/cache' + ' ; mv /tmp/cache '+ restLfnFile
        os.system(cmd)

        print ' MISSING - Lfns: %6d'%(self.nLfnMissing)

    #-----------------------------------------------------------------------------------------------
    # create an inventory of all the existing output files
    #-----------------------------------------------------------------------------------------------
    def makeInventory(self):
        castorPath = self.storagePath
        f = castorPath.split("=")
        castorPath = f[1]
        #cmd = "srmls srm://" + self.storageEle + ":8443" + self.storagePath + " | grep " \
        #      + self.mitDataset + "_"
        cmd = "list " + castorPath + " | grep root | cut -d ' ' -f2"
        
        print " cmd: " + cmd
        for status in self.jobStati:
            status.outputFile = 0
        for line in os.popen(cmd).readlines():    # run directory list command
            #print " line: " + line
            line   = line[:-1]                    # strip '\n'
            f      = line.split("_")
            number = f.pop()
            f      = number.split(".")
            number = int(f[0])
            # update the job status
            #print ' Index: %d >> %s'%(number,line)
            if number-1 >= len(self.jobStati):
                print ' Index out of range requested: %d  Waiting for the CRASH!'%(number)
            self.jobStati[number-1].outputFile = 1
    #-----------------------------------------------------------------------------------------------
    # get crab status information for each job in the task
    #-----------------------------------------------------------------------------------------------
    def getJobStati(self):
        # Interact with crab to determine the present status of the jobs
        pattern = 'Created'
        # result = re.search(pattern,line)

        active        = 0
        self.jobStati = []
        self.failingSites  = {}
        cmd = 'crab -status -continue ' + self.tag
        #print 'Access Crab Job Stati, now!'
        for line in os.popen(cmd).readlines():  # run command
            line       = line[:-1]              # strip '\n'
            if active == 0:
                print ' CRAB: ' + line
            else:
                if not re.search(pattern,line) and not re.search('------',line):
                    print ' CRAB: ' + line

            #>> # compactify line
            #>> line       = " ".join(str(line).split()).strip()
            #>> f          = line.split(" ")
            # decide whether we are in job status line or not
            if line[1:5] == "----":
                if active == 0:
                    active = 1  # print "Activated parsing"
                continue
            if active == 1 and line == '':
                active = 0      # print "Deactivated parsing"
            # parse the content of the job report
            if active == 1:
            #>>     #print ' LINE: ' + line
            #>>     status = JobStatus(int(f[0]),f[1])
            #>>     if len(f) > 2:
            #>>         status.ce         = f[2]
            #>> 
            #>>         if len(f) >= 2 and f[1] == 'Retrieved':
            #>>             if len(f) > 5:
            #>>                 status.exitCode   = int(f[3])
            #>>                 status.exitStatus = int(f[4])

            # fixed column read
##ID     STATUS             E_HOST                               EXE_EXIT_CODE JOB_EXIT_STATUS 
                #lastChar          = len(line)
                #print 'Last Char: %d'%lastChar
                iJob              = int(line[0:5].strip())
                sJob              = line[7:24].strip()
                status            = JobStatus(iJob,sJob)
                status.ce         = line[26:61].strip()
                tmp               = line[63:75].strip()
                if tmp != '':
                    status.exitCode   = int(tmp)
                tmp               = line[77:91].strip()
                if tmp != '':
                    status.exitStatus = int(tmp)

                #print ' Appending:  id %d  array entry: %d '%(iJob,len(self.jobStati))
                self.jobStati.append(status)

        # review job output so far
        if len(self.jobStati) > 0:
            self.makeInventory()
        else:
            print ' ERROR - This task has not jobs stati assigned to it. Something is wrong.'
            print '         crab task id: ' + self.tag


        # Make sure certain cases get fixed to avoid deletion
        for status in self.jobStati:
            # fix those jobs where the output has already been found
            if   status.exitStatus == 60303 and status.outputFile == 1:
                status.exitStatus = 0
            elif status.exitStatus == -1 and status.exitCode == -1 and status.outputFile == 1:
                status.exitStatus = 0
                status.exitCode   = 0

            elif (status.exitStatus != 0 or status.exitCode != 0) and \
                     not (status.exitStatus == -1 and status.exitCode == -1):

                print "  ==> Failure: filing with status/code:  %d  %d  CE: %s" \
                      %(status.exitStatus,status.exitCode,status.ce)

                if status.ce in self.failingSites:
                    self.failingSites[status.ce] += 1
                else:
                    self.failingSites[status.ce]  = 1

        #print ' Dimension of Job Stati: %d'%(len(self.jobStati))

        # Loop through the job stati and determine the task status
        # - check whether task is completed
        active = 0
        for status in self.jobStati:
            if status.tag != 'Aborted' and status.tag != 'Retrieved' and status.tag != 'Created':
                active = 1
                break
        if active == 0:
            self.status = 'completed'
            for status in self.jobStati:
                if status.tag == 'Aborted' or status.exitCode != 0 or status.exitStatus != 0:
                    self.status = 'finished'
                    break
        else:
            self.status = 'active'
        


    #-----------------------------------------------------------------------------------------------
    # print the line to complete the task
    #-----------------------------------------------------------------------------------------------
    def complete(self):
        print ' ./bin/submit.py --noTestJob --complete --version=008 --mitDataset=%s'% \
              (self.mitDataset)

    #-----------------------------------------------------------------------------------------------
    # print the line to remove the task and do it if requested
    #-----------------------------------------------------------------------------------------------
    def remove(self,clean=0):
        cmd = ' cleanupLog.py --crabId ' + self.tag + \
              '; mkdir -p ./completed; mv ' + self.tag + ' ./completed/'
        print ' -> ' + cmd
        if clean == 1:
            os.system(cmd)

    #-----------------------------------------------------------------------------------------------
    # print the line to remove the task and do it if requested
    #-----------------------------------------------------------------------------------------------
    def killAndRemove(self,clean=0):
        cmd = 'crab -kill all -continue ' +self.tag + '; cleanupLog.py --crabId=' + self.tag \
              + '; mkdir -p ./completed; mv ' + self.tag + ' ./completed/'
        print ' -> ' + cmd
        if clean == 1:
            os.system(cmd)
        
#---------------------------------------------------------------------------------------------------
"""
Class:  JobStatus(index,tag)
The status of one job of a list of CRAB jobs (inside one task) is described by this class
"""
#---------------------------------------------------------------------------------------------------
class JobStatus:
    "Minimal but sufficient Job Status for crab operations"
    index      = -1
    tag        = 'undefined'
    ce         = 'undefined'
    outputFile = -1
    exitCode   = -1
    exitStatus = -1
    #-----------------------------------------------------------------------------------------------
    # constructor
    #-----------------------------------------------------------------------------------------------
    def __init__(self, index, tag):
        self.index = index
        self.tag   = tag
    #-----------------------------------------------------------------------------------------------
    # present the current crab task in compact form
    #-----------------------------------------------------------------------------------------------
    def showCompact(self):
        print 'Status: %6d %20s   Output: %2d Exit: %6d,%6d  at CE: %s'% \
              (self.index,self.tag,self.outputFile,self.exitCode, \
               self.exitStatus,self.ce)
    #-----------------------------------------------------------------------------------------------
    # present the current crab task in long form
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print '==== Job Status Information ===='
        print ' Index: %6d  Tag: %s  CE: %s'%(self.index,self.tag,self.ce)
        print ' Output:    %2d  Exit: %6d,%6d'%(self.outputFile,self.exitCode,self.exitStatus)
