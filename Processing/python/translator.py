#---------------------------------------------------------------------------------------------------
# Python Module File to translate storage/computing elements into meta site names
#
# Author: C.Paus                                                                      (Jun 16, 2010)
#---------------------------------------------------------------------------------------------------
import os,sys,re,string

#---------------------------------------------------------------------------------------------------
"""
Class:  Translator(table)
Each SubTask in CRAB can be described through this class
"""
#---------------------------------------------------------------------------------------------------
class Translator:
    "Translator for the storage and computing elements from hostnames to generic site names"

    # variable to be determined
    ceTable            = 'undefined' # compute elements translation table
    seTable            = 'undefined' # storage elements translation table
    allSites           = 'undefined' # comma separated list of all sites
    prefSitesTable     = 'undefined' # comma separated list of preferred sites
    ces                = {}
    ses                = {}
    sites              = []          # generic name of all sites
    preferredSitesList = []
    preferredSites     = []
    #-----------------------------------------------------------------------------------------------
    # constructor to connect with existing setup
    #-----------------------------------------------------------------------------------------------
    def __init__(self,ceTable,seTable,prefSitesTable):
        self.ceTable        = ceTable
        self.seTable        = seTable
        self.prefSitesTable = prefSitesTable
        if os.path.exists(self.seTable):
            self.ses = self.readTable(self.seTable)
        else:
            print ' WARNING -- SE table file not found.'
        if os.path.exists(self.ceTable):
            self.ces = self.readTable(self.ceTable)
        else:
            print ' WARNING -- CE table file not found.'
        if os.path.exists(self.prefSitesTable):
            self.preferredSitesList = self.readList(self.prefSitesTable)
        else:
            print ' WARNING -- prefSites list file not found.'

        #self.show()

    def readList(self,file):
        # read a given file name as a list each line one list entry       

        list = []
        cmd = 'grep -v ^# ' + file + ' | tr -d \' \''
        print ' Loading list: ' + cmd
        for line in os.popen(cmd).readlines():
            line  = line[:-1]
            # decode the storage directory name
            names = line.split(" ")
            list.append(names[0])

        return list

    def readTable(self,file):
        # read a given file name as a table with colon separated entries
        
        table = {}
        cmd = 'grep -v ^# ' + file + ' | tr -d \' \''
        print ' Loading table: ' + cmd
        for line in os.popen(cmd).readlines():
            line         = line[:-1]
            # decode the storage directory name
            names        = line.split(":")
            if names[1] in table:
                print ' Site already in tables %s (%s)'%(names[1],names[0])
            table[names[1]] = names[0]

            # add all generic site names
            if not (names[0] in self.sites):
                self.sites.append(names[0])

        return table

    def cleanSites(self,seString):
        # not all sites are available we will only keep the ones specified in out table

        print ' translating SE sites: ' + seString
        sites = seString.split(",")
        newString = ''
        for site in sites:
            if site in self.sites:
                if newString == '':
                    newString = site
                else:
                    newString = "%s,%s"%(newString,site)

        #print 'Cleaned: %s'%(newString)
        self.allSites = newString
        return newString

    def translateSes(self,seString):
        # translate the given string from SE hostnames to the generic site names (T2_US_MIT, etc.)

        print ' translating SE sites: ' + seString
        self.show()

        sites = seString.split(",")
        newString = ''
        #print ' Looping through sites %s'%(seString)
        for site in sites:
            if site in self.ses:
                #print '   ->  %s  ==  %s'%(site,self.ses[site])
                if newString == '':
                    newString = self.ses[site]
                else:
                    newString = "%s,%s"%(newString,self.ses[site])
            else:
                print '  dropping  %s  because it is not in our table'%(site)

        #print 'Translated: %s'%(newString)
        self.allSites = newString
        return newString

    def translateCes(self,ceString):
        # translate the given string from SE hostnames to the generic site names (T2_US_MIT, etc.)

        print ' translating CE sites: ' + ceString
        sites = ceString.split(",")
        newString = ''
        #print ' Looping through sites %s'%(ceString)
        for site in sites:
            if site in self.ces:
                #print '   ->  %s  ==  %s'%(site,self.ces[site])
                if newString == '':
                    newString = self.ces[site]
                else:
                    newString = "%s,%s"%(newString,self.ces[site])
            else:
                print '  dropping  %s  because it is not in our table'%(site)

        
        #print 'Translated: %s'%(newString)
        self.allSites = newString
        return newString

    def selectPreferred(self):
        #preferredSitesList = ['T2_US_MIT','T2_US_Wisconsin']
        self.preferredSites = ''
        for site in self.preferredSitesList:
            if re.search(site,self.allSites):
                if self.preferredSites != '':
                    self.preferredSites = self.preferredSites + ','
                self.preferredSites = self.preferredSites + site
     
        if self.preferredSites != '':
            print ' '
            print '         ' + self.allSites
            print ' INFO -- ' + self.preferredSites + ' found a preferred site (reducing).'
            print ' '
        else:
            #print ' ' 
            #print ' INFO -- no preferred site in the list. Keeping all sites.'
            #print ' '
            self.preferredSites = self.allSites
        return self.preferredSites    
            
    #-----------------------------------------------------------------------------------------------
    # present the current crab subtask
    #-----------------------------------------------------------------------------------------------
    def show(self):
        print ' Translator (CE Table: %s,  SE Table: %s) ===='%(self.ceTable,self.seTable)
        print ' ==== Compute Elements'
        for name,hostname in self.ces.iteritems():
            print '   Site: %s  -->  CE: %s'%(name,hostname)
        print ' ==== Storage Elements'
        for name,hostname in self.ses.iteritems():
            print '   Site: %s  -->  SE: %s'%(name,hostname)
