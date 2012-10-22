#---------------------------------------------------------------------------------------------------
# Python Module File to describe files, filesets and fileblocks
#
# File:      the smallest unit of a bunch of data which are accounted for on a hard drive.
# Fileset:   a set of files which contain the same type of events and are stored in the same
#            directory. Filesets are created to reduce the number of 'processing jobs' when talking
#            about greater collections of data. The number of files per fileset is therefore
#            crucial.
# Fileblock: all files which possibly are represented in a bunch of filesets which contain the same
#            type of events and reside on the same hard drive.
#
# Author: C.Paus                                                                      (Oct 10, 2008)
#---------------------------------------------------------------------------------------------------
import os,sys,getopt,re,string

#---------------------------------------------------------------------------------------------------
"""
Class:  MetaData(nEvents,nLumiSecs,minRun,minLumiSecInMinRun,maxRun,maxLumiSecInMaxRun)

A given bunch of events, may it be a file or just 10 events or even an entire dataset can be
specified by minimal set of meta data. This class defines this set of meta data and the methods
useful for dealing with it.
"""
#---------------------------------------------------------------------------------------------------
class MetaData:
    "A Meta Data description of a bunch of Bambu events (any data/MC events can be described)."
    # this is sufficient to do anything
    nEvents            = 0
    nLumiSecs          = 0
    minRun             = 999999999
    minLumiSecInMinRun = 0
    maxRun             = 0
    maxLumiSecInMaxRun = 0
    #-----------------------------------------------------------------------------------------------
    # constructor to connect with existing setup
    #-----------------------------------------------------------------------------------------------
    def __init__(self,nEvents,nLumiSecs,minRun,minLumiSecInMinRun,maxRun,maxLumiSecInMaxRun):
        self.nEvents            = nEvents
        self.nLumiSecs          = nLumiSecs
        self.minRun             = minRun
        self.minLumiSecInMinRun = minLumiSecInMinRun
        self.maxRun             = maxRun
        self.maxLumiSecInMaxRun = maxLumiSecInMaxRun
    #-----------------------------------------------------------------------------------------------
    # present the current set of Meta Data
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print ' Number of:    events %9d  lumi sections %d'%(self.nEvents,self.nLumiSecs)
        print ' Lowest  data: run    %9d  lumi section  %9d'%(self.minRun,self.minLumiSecInMinRun)
        print ' Highest data: run    %9d  lumi section  %9d'%(self.maxRun,self.maxLumiSecInMaxRun)
    #-----------------------------------------------------------------------------------------------
    # adding meta data
    #-----------------------------------------------------------------------------------------------
    def add(self,right):
        self.nEvents           += right.nEvents
        self.nLumiSecs         += right.nLumiSecs
        if self.minRun > right.minRun or \
               (self.minRun == right.minRun and \
                self.minLumiSecInMinRun > right.minLumiSecInMinRun):
            self.minRun             = right.minRun
            self.minLumiSecInMinRun = right.minLumiSecInMinRun
        if self.maxRun < right.maxRun or \
               (self.maxRun == right.maxRun and \
                self.maxLumiSecInMaxRun < right.maxLumiSecInMaxRun):
            self.maxRun             = right.maxRun
            self.maxLumiSecInMaxRun = right.maxLumiSecInMaxRun

#---------------------------------------------------------------------------------------------------
"""
Class:  File(name,nEvents,nLumiSecs,minRun,minLumiSecInMinRun,maxRun,maxLumiSecInMaxRun)

This class defines the Meta Data relevant for a file.
"""
#---------------------------------------------------------------------------------------------------
class File(MetaData):
    "A Meta Data description of a Bambu File (any data/MC file can be described this way)."
    # this is sufficient to do anything
    name               = 'undefined'
    #-----------------------------------------------------------------------------------------------
    # constructor to connect with existing setup
    #-----------------------------------------------------------------------------------------------
    def __init__(self,nEvents,nLumiSecs,minRun,minLumiSecInMinRun,maxRun,maxLumiSecInMaxRun,name):
        MetaData.__init__(self,nEvents,nLumiSecs,minRun,minLumiSecInMinRun,maxRun,maxLumiSecInMaxRun)
        self.name = name
    #-----------------------------------------------------------------------------------------------
    # present the current crab task
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print ' ==== Meta Data for File %s ===='%(self.name)
        MetaData.show(self)

    def showShort(self,filesetName,oFile=0,debug=1):
        line = "%s %40s %9d %9d %9d %6d %9d %6d"%(filesetName,self.name, \
                                                  self.nEvents,self.nLumiSecs, \
                                                  self.minRun,self.minLumiSecInMinRun, \
                                                  self.maxRun,self.maxLumiSecInMaxRun)
        if debug > 0:
            print line
        oFile.write(line+'\n')

#---------------------------------------------------------------------------------------------------
"""
Class:  Fileset(name)

This class defines the Meta Data relevant for a set of files. They will be extracted from the files
being added to the set.
"""
#---------------------------------------------------------------------------------------------------
class Fileset(MetaData):
    "A Meta Data description of a set of Bambu Files (any data/MC file can be described this way)."
    # this is sufficient to do anything
    name               = 'undefined'
    dir                = ''
    fileList           = []
    #-----------------------------------------------------------------------------------------------
    # constructor to connect with existing setup
    #-----------------------------------------------------------------------------------------------
    def __init__(self,name,dir):
        MetaData.__init__(self,0,0,999999999,0,0,0)
        self.name = name
        self.dir  = dir
        fileList  = []
    #-----------------------------------------------------------------------------------------------
    # resetting it to zero
    #-----------------------------------------------------------------------------------------------
    def reset(self,name,dir):
        MetaData.__init__(self,0,0,999999999,0,0,0)
        self.name     = name
        self.dir      = dir
        self.fileList = []
    #-----------------------------------------------------------------------------------------------
    # present the contents of the fileset in various forms
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print ' ==== Meta Data for Fileset %s ===='%(self.name)
        MetaData.show(self)
    def showShort(self,oFile=0,debug=1):
        line = "%s %40s %9d %9d %9d %6d %9d %6d"%(self.name,self.dir,self.nEvents,self.nLumiSecs, \
                                                 self.minRun,self.minLumiSecInMinRun, \
                                                 self.maxRun,self.maxLumiSecInMaxRun)
        if debug > 0:
            print line
        oFile.write(line+'\n')
    def showShortFiles(self,oFile=0,debug=1):
        for file in self.fileList:
            file.showShort(self.name,oFile,debug)
    #-----------------------------------------------------------------------------------------------
    # add one more file
    #-----------------------------------------------------------------------------------------------
    def addFile(self,file):
        self.add(file)
        self.fileList.append(file)
    #-----------------------------------------------------------------------------------------------
    # how many files in this set
    #-----------------------------------------------------------------------------------------------
    def nFiles(self):
        return len(self.fileList)
