//--------------------------------------------------------------------------------------------------
// $Id: FillerGlobalMuons.h,v 1.3 2008/06/18 19:17:21 loizides Exp $
//
// FillerPhotons
//
// Implementation of a filler to fill photons into our data structure.  (Including converted photons.)
//
// Authors: J. Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERPHOTONS_H
#define TREEFILLER_FILLERPHOTONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitAna/DataTree/interface/Photon.h"
#include "MitAna/DataTree/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "MitAna/DataTree/interface/Array.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMap.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

using namespace std;
using namespace mithep;



namespace mithep 
{
  class FillerPhotons : public BaseFiller
  {  
    public:
      FillerPhotons(const edm::ParameterSet&, bool active, const ConversionMap* conversionMap);
      ~FillerPhotons();

      void                                       BookDataBlock(TreeWriter &tws);
      void                                       FillDataBlock(const edm::Event&, const edm::EventSetup&);
  
    private:
      std::string                                edmName_;
      std::string                                mitName_;
      mithep::PhotonArr                         *photons_;
      edm::Handle<reco::PhotonCollection>        photonProduct_;
      const mithep::ConversionMap*               conversionMap_;
  };
}
#endif
