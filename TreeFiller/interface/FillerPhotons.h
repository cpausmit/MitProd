//--------------------------------------------------------------------------------------------------
// $Id: FillerPhotons.h,v 1.2 2008/07/03 07:56:14 loizides Exp $
//
// FillerPhotons
//
// Implementation of a filler to fill photons into our data structure,
// Including converted photons.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERPHOTONS_H
#define TREEFILLER_FILLERPHOTONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerPhotons : public BaseFiller
  {  
    public:
      FillerPhotons(const edm::ParameterSet &cfg, bool active=1, 
                    const ConversionMap *conversionMap=0);
      ~FillerPhotons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                                edmName_;
      std::string                                mitName_;
      mithep::PhotonArr                         *photons_;
      edm::Handle<reco::PhotonCollection>        photonProduct_;
      const mithep::ConversionMap*               conversionMap_;
  };
}
#endif
