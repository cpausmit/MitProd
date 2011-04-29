//--------------------------------------------------------------------------------------------------
// $Id: FillerPFTaus.h,v 1.6 2011/01/27 12:36:46 mhchan Exp $
//
// FillerPFTaus
//
// Implementation of a filler to fill EDM pf taus into our mithep::PFTau data structure.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFTAUS_H
#define MITPROD_TREEFILLER_FILLERPFTAUS_H

#include "MitAna/DataTree/interface/PFTauFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

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
      bool                           hpsActive_;      //=true if HPS discriminants are filled
      std::string                    edmName_;        //edm name of jets collection
      std::string                    mitName_;        //mit name of jets collection
      std::string                    discriminationByLooseElectronRejectionName_; // HPS discriminant
      std::string                    discriminationByMediumElectronRejectionName_; // HPS discriminant
      std::string                    discriminationByTightElectronRejectionName_; // HPS discriminant
      std::string                    discriminationByLooseMuonRejectionName_; // HPS discriminant
      std::string                    discriminationByTightMuonRejectionName_; // HPS discriminant
      std::string                    discriminationByDecayModeFindingName_; // HPS discriminant
      std::string                    discriminationByVLooseIsolationName_;   // HPS discriminant
      std::string                    discriminationByLooseIsolationName_;   // HPS discriminant
      std::string                    discriminationByMediumIsolationName_;  // HPS discriminant
      std::string                    discriminationByTightIsolationName_;   // HPS discriminant
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
