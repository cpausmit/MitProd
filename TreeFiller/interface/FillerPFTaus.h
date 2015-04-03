//--------------------------------------------------------------------------------------------------
// FillerPFTaus
//
// Implementation of a filler to fill EDM pf taus into our mithep::PFTau data structure.
//
// Authors: C.Loizides, J.Bendavid, C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFTAUS_H
#define MITPROD_TREEFILLER_FILLERPFTAUS_H

#include "MitAna/DataTree/interface/PFTauFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"
#include "DataFormats/TauReco/interface/PFTauFwd.h"

namespace mithep
{
  class FillerPFTaus : public BaseFiller
  {
    public:
      FillerPFTaus(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerPFTaus();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      const mithep::Track *getMitTrack(mitedm::TrackPtr ptr, bool allowmissing) const;

      typedef std::map<std::string, edm::EDGetTokenT<reco::PFTauDiscriminator> > DiscTokenMap;

      bool                           hpsActive_;      //=true if HPS discriminants are filled
      edm::EDGetTokenT<reco::PFTauCollection> edmToken_;        //edm name of jets collection
      DiscTokenMap hpsTokens_;
      std::string                    mitName_;        //mit name of jets collection
      std::vector<std::string>       trackMapNames_;   //name of imported TrackMap
      std::string                    jetMapName_;     //name of imported PFJetMap
      std::string                    pfCandMapName_;  //name of imported PFCandidateMap
      std::string                    tauMapName_;     //name of exported PFTau Map
      bool                           allowMissingTrackRef_; //allow missing track reference
                                                            //-->needed for tau embedding samples
      std::vector<const mithep::TrackMap*> trackMaps_;       //map wrt Tracks
      const mithep::PFJetMap        *jetMap_;         //map wrt pfjets
      const mithep::PFCandidateMap  *pfCandMap_;      //map wrt pf candidates
      mithep::PFTauMap              *tauMap_;         //exported PFTau map
      mithep::PFTauArr              *taus_;           //array of taus
  };
}
#endif
