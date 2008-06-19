// $Id: FillerMetaInfos.cc,v 1.3 2008/06/18 19:17:21 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerMetaInfos::FillerMetaInfos(const ParameterSet &cfg) : 
  BaseFiller(cfg,"MetaInfos")
{
}

//-------------------------------------------------------------------------------------------------
FillerMetaInfos::~FillerMetaInfos()
{
}

//-------------------------------------------------------------------------------------------------
void FillerMetaInfos::BookDataBlock(TreeWriter *tws)
{
  if (! tws) {
    throw edm::Exception(edm::errors::Configuration, "FillerMetaInfos::BookDataBlock()\n")
      << "Invalid pointer.\n";
    return;
  }
//  tws->AddBranch(mitName_.c_str(),&muons_);
}

//-------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillDataBlock(const edm::Event      &event, 
                                    const edm::EventSetup &setup)
{

}

//-------------------------------------------------------------------------------------------------
void FillerMetaInfos::ResolveLinks(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{

}
