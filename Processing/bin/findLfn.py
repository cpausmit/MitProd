#!/usr/bin/env python
#---------------------------------------------------------------------------------------------------
# Stole this from Valery [Author:  Valentin Kuznetsov, 2008]
#
# Probably it could do everything I wanted but I hacked it together.
#---------------------------------------------------------------------------------------------------
import os,sys,types,string
import httplib,urllib
from optparse import OptionParser

"""
DBS data discovery command line interface
"""

class DDOptionParser: 
  """
        DDOptionParser is main class to parse options for L{DDHelper} and L{DDServer}.
  """
  def __init__(self):
    self.parser = OptionParser()
    self.parser.add_option("--dbsInst",action="store", type="string", dest="dbsInst",
                           help="specify DBS instance to use, e.g. --dbsInst=cms_dbs_prod_global")
    self.parser.add_option("-v","--verbose",action="store", type="int", default=0, dest="verbose",
                           help="specify verbosity level, 0-none, 1-info, 2-debug")
    self.parser.add_option("--input",action="store", type="string", default=False, dest="input",
                           help="specify input for your request.")
    self.parser.add_option("--xml",action="store_true",dest="xml",
                           help="request output in XML format")
    self.parser.add_option("--cff",action="store_true",dest="cff",
                           help="request output for files in CMS cff format")
    self.parser.add_option("--host",action="store",type="string",dest="host",
                           help="specify a host name of Data Discovery service, e.g. https://cmsweb.cern.ch/dbs_discovery/")
    self.parser.add_option("--details",action="store_true",dest="details",
                           help="show detailed output")
    self.parser.add_option("--case",action="store",default="on",type="string",dest="case",
                           help="specify if your input is case sensitive of not, default is on.")
    self.parser.add_option("--page",action="store",type="string",default="0",dest="page",
                           help="specify output page, should come together with --limit and --details")
    self.parser.add_option("--limit",action="store",type="string",default="10",dest="limit",
                           help="specify a limit on output, e.g. 50 results, the --limit=-1 will list all results")
  def getOpt(self):
    """
        Returns parse list of options
    """
    return self.parser.parse_args()

def sendMessage(host,port,dbsInst,userInput,page,limit,xml=0,case='on',details=0,cff=0,debug=0):
  """
  Send message to server, message should be an well formed XML document.
  """
  if xml:
    xml=1
  else:
    xml=0
  if cff:
    cff=1
  else:
    cff=0
    input = urllib.quote(userInput)

    if debug:
      httplib.HTTPConnection.debuglevel = 1
      print "Contact",host,port
    _port=443
    if host.find("http://") != -1:
      _port=80
    if host.find("https://") != -1:
      _port=443
    host=host.replace("http://","").replace("https://","")
    if host.find(":")==-1:
       port=_port
    prefix_path=""
    if host.find("/")!=-1:
       hs=host.split("/")
       host=hs[0]
       prefix_path='/'.join(hs[1:])
    port=int(port)
    if port==443:
       http_conn = httplib.HTTPS(host,port)
    else:
       http_conn = httplib.HTTP(host,port)
    if details:
      details=1
    else:
      details=0

    path='/getLFNsForSite?dbsInst=%s&site=*&datasetPath=%s&what=txt&userMode=user&run=*' \
          %(dbsInst,input)

##    path='/aSearch?dbsInst=%s&html=0&caseSensitive=%s&_idx=%s&pagerStep=%s&userInput=%s&xml=%s&details=%s&cff=%s'%(dbsInst,case,page,limit,input,xml,details,cff)

    if prefix_path:
       path="/"+prefix_path+path[1:]

#    print ' HOST: ' + host
#    print ' PATH: ' + path
    http_conn.putrequest('POST',path)
    http_conn.putheader('Host',host)
    http_conn.putheader('Content-Type','text/html; charset=utf-8')
    http_conn.putheader('Content-Length',str(len(input)))
    http_conn.endheaders()

#    print ' IPUT: ' + input
    http_conn.send(input)

    (status_code,msg,reply) = http_conn.getreply()
    data = http_conn.getfile().read()
    if debug or msg != "OK":
      print
      print http_conn.headers
      print "*** Send message ***"
      print input
      print "************************************************************************"
      print "status code:",status_code
      print "message:",msg
      print "************************************************************************"
      print reply
    return data

#---------------------------------------------------------------------------------------------------
# main
#---------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  host= "cmsweb.cern.ch/dbs_discovery/"
  port= 443
  dbsInst="cms_dbs_prod_global"
  optManager  = DDOptionParser()
  (opts,args) = optManager.getOpt()
  if opts.host:
    host=opts.host
  if host.find("http://") != -1:
    host=host.replace("http://","")
  if host[-1] != "/":
    host += "/"
  if opts.dbsInst:
    dbsInst = opts.dbsInst
  if opts.input:
    if os.path.isfile(opts.input):
      input = open(opts.input,'r').readline()
    else:
      input = opts.input
  else:
    print "\nUsage: %s --help"%sys.argv[0]
    sys.exit(0)

  result = sendMessage(host,port,dbsInst,input,
                       opts.page,opts.limit,opts.xml,opts.case,opts.details,opts.cff,opts.verbose)
  print result
