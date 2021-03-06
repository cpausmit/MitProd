#!/usr/bin/env python
#pylint: disable-msg=C0301,C0103

"""
DAS command line tool
"""
__author__ = "Valentin Kuznetsov"

import sys
if  sys.version_info < (2, 6):
    raise Exception("DAS requires python 2.6 or greater")

import re
import time
import json
import urllib
import urllib2
from   optparse import OptionParser

class DASOptionParser: 
    """
    DAS cache client option parser
    """
    def __init__(self):
        self.parser = OptionParser()
        self.parser.add_option("-v", "--verbose", action="store", 
                               type="int", default=0, dest="verbose",
             help="verbose output")
        self.parser.add_option("--query", action="store", type="string", 
                               default=False, dest="query",
             help="specify query for your request")
        self.parser.add_option("--host", action="store", type="string", 
                               default='https://cmsweb.cern.ch', dest="host",
             help="specify host name of DAS cache server, default https://cmsweb.cern.ch")
        self.parser.add_option("--idx", action="store", type="int", 
                               default=0, dest="idx",
             help="start index for returned result set, aka pagination, use w/ limit")
        self.parser.add_option("--limit", action="store", type="int", 
                               default=10, dest="limit",
             help="number of returned results (results per page)")
        self.parser.add_option("--format", action="store", type="string", 
                               default="json", dest="format",
             help="specify return data format (json or plain), default json")
    def get_opt(self):
        """
        Returns parse list of options
        """
        return self.parser.parse_args()

def get_value(data, filters):
    """Filter data from a row for given list of filters"""
    for ftr in filters:
        if  ftr.find('>') != -1 or ftr.find('<') != -1 or ftr.find('=') != -1:
            continue
        row = dict(data)
        for key in ftr.split('.'):
            if  isinstance(row, dict) and row.has_key(key):
                row = row[key]
            if  isinstance(row, list):
                for item in row:
                    if  isinstance(item, dict) and item.has_key(key):
                        row = item[key]
                        break
        yield str(row)

def get_data(host, query, idx, limit, debug):
    """Contact DAS server and retrieve data for given DAS query"""
    params  = {'input':query, 'idx':idx, 'limit':limit}
    path    = '/das/cache'
    pat     = re.compile('http[s]{0,1}://')
    if  not pat.match(host):
        msg = 'Invalid hostname: %s' % host
        raise Exception(msg)
    url = host + path
    headers = {"Accept": "application/json"}
    encoded_data = urllib.urlencode(params, doseq=True)
    url += '?%s' % encoded_data
    req  = urllib2.Request(url=url, headers=headers)
    if  debug:
        hdlr = urllib2.HTTPHandler(debuglevel=1)
        opener = urllib2.build_opener(hdlr)
    else:
        opener = urllib2.build_opener()
    fdesc = opener.open(req)
    data = fdesc.read()
    fdesc.close()

    pat = re.compile(r'^[a-z0-9]{32}')
    if  data and isinstance(data, str) and pat.match(data) and len(data) == 32:
        pid = data
    else:
        pid = None
    count = 5  # initial waiting time in seconds
    timeout = 30 # final waiting time in seconds
    while pid:
        params.update({'pid':data})
        encoded_data = urllib.urlencode(params, doseq=True)
        url  = host + path + '?%s' % encoded_data
        req  = urllib2.Request(url=url, headers=headers)
        try:
            fdesc = opener.open(req)
            data = fdesc.read()
            fdesc.close()
        except urllib2.HTTPError, err:
            print err
            return ""
        if  data and isinstance(data, str) and pat.match(data) and len(data) == 32:
            pid = data
        else:
            pid = None
        time.sleep(count)
        if  count < timeout:
            count *= 2
        else:
            count = timeout
    return data

def main():
    """Main function"""
    optmgr  = DASOptionParser()
    opts, _ = optmgr.get_opt()
    host    = opts.host
    debug   = opts.verbose
    query   = opts.query
    idx     = opts.idx
    limit   = opts.limit
    if  not query:
        raise Exception('You must provide input query')
    data    = get_data(host, query, idx, limit, debug)
    if  opts.format == 'plain':
        jsondict = json.loads(data)
        mongo_query = jsondict['mongo_query']
        if  mongo_query.has_key('filters'):
            filters = mongo_query['filters']
            data = jsondict['data']
            if  isinstance(data, dict):
                rows = [r for r in get_value(data, filters)]
                print ' '.join(rows)
            elif isinstance(data, list):
                for row in data:
                    rows = [r for r in get_value(row, filters)]
                    print ' '.join(rows)
            else:
                print jsondict
    elif opts.format == 'blocks':
        array = data.split(',')
        for d in array:
            if re.search("block",d) and re.search("#",d):
                l = d.split('"')
                block = l[5]
                print block
    elif opts.format == 'files':
        print "Output from Query with option 'files' ....."
        #print data
        list = data.split(',')
        for d in list:
            # decode the file name that comes first
            if re.search("block",d):
                l = d.split('"')
                block = l[3]
            if re.search(".root",d) and not re.search("{\"name\":",d):
                l = d.split('"')
                file = l[3]
            if re.search("nevents",d):
                l = d.split('"')
                nevents = l[2]
                nevents = nevents[2:-2]
                print file + " " + nevents
    elif opts.format == 'all':
        list = data.split(',')
        for d in list:
            # decode the file name that comes first
            if re.search("block",d):
                l = d.split('"')
                block = l[3]
            if re.search(".root",d) and not re.search("{\"name\":",d):
                l = d.split('"')
                file = l[3]
            if re.search("nevents",d):
                l = d.split('"')
                nevents = l[2]
                nevents = nevents[2:-2]
                print block + " " + file + " " + nevents
    else:
        print "Output from Query....."
        print data

#
# main
#
if __name__ == '__main__':
    main()
