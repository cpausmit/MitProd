//--------------------------------------------------------------------------------------------------
// $Id: FillerPFTaus.h,v 1.1 2009/03/20 18:47:45 bendavid Exp $
//
// FillerPFTaus
//
// Implementation of a filler to fill EDM pf taus into our mithep::PFTau data structure.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFTAUS_H
#define MITPROD_TREEFILLER_FILLERPFTAUS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerPFTaus : public BaseFiller
  {  
    public:
      FillerPFTaus(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPFTaus();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                    edmName_;        //edm name of jets collection
      std::string                    mitName_;        //mit name of jets collection
      std::string                    trackMapName_;   //name of imported TrackMap
      std::string                    jetMapName_;     //name of imported PFJetMap
      std::string                    pfCandMapName_;  //name of imported PFCandidateMap
      const mithep::TrackMap        *trackMap_;       //map wrt Tracks
      const mithep::PFJetMap        *jetMap_;         //map wrt pfjets
      const mithep::PFCandidateMap  *pfCandMap_;      //map wrt pf candidates
      mithep::PFTauArr              *taus_;           //array of taus
  };
}
#endif
