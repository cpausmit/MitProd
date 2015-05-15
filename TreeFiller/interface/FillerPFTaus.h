//--------------------------------------------------------------------------------------------------
// FillerPFTaus
//
// Implementation of a filler to fill EDM taus into our mithep::PFTau data structure.
// Source tau class can be either reco::PFTau or pat::Tau, which share only the base class
// (unlike other leptons where standard reco:: object is the base class of the pat:: object).
//
// Authors: C.Loizides, J.Bendavid, C.Paus, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFTAUS_H
#define MITPROD_TREEFILLER_FILLERPFTAUS_H

#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"

#include "MitAna/DataTree/interface/PFTauCol.h"

namespace mithep
{
  template<class TAU>
  class FillerTaus : public BaseFiller
  {
    public:
      FillerTaus(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerTaus();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

      typedef std::vector<TAU> TauCollection;

    private:
      typedef std::map<std::string, edm::EDGetTokenT<reco::PFTauDiscriminator> > DiscTokenMap;
      typedef std::map<std::string, edm::Handle<reco::PFTauDiscriminator> > DiscHandleMap;

      const mithep::Track *getMitTrack(mitedm::TrackPtr ptr, bool allowmissing) const;
      void setHPSTokens(edm::ConsumesCollector&);
      void setPFTauDiscriminators(mithep::PFTau*, DiscHandleMap const&, edm::Ref<TauCollection> const&) const;
      void setPFJetRef(mithep::PFTau*, TAU const&) const;

      bool                            hpsActive_;      //=true if HPS discriminants are filled
      edm::EDGetTokenT<TauCollection> edmToken_;        //edm name of jets collection
      DiscTokenMap                    hpsTokens_;
      std::string                     mitName_;        //mit name of jets collection
      std::vector<std::string>        trackMapNames_;   //name of imported TrackMap
      std::string                     jetMapName_;     //name of imported PFJetMap
      std::string                     pfCandMapName_;  //name of imported PFCandidateMap
      std::string                     tauMapName_;     //name of exported PFTau Map
      bool                            allowMissingTrackRef_; //allow missing track reference
                                                            //-->needed for tau embedding samples
      std::vector<const mithep::TrackMap*> trackMaps_;       //map wrt Tracks
      const mithep::PFJetMap        *jetMap_;         //map wrt pfjets
      const mithep::PFCandidateMap  *pfCandMap_;      //map wrt pf candidates
      mithep::PFTauMap              *tauMap_; //exported PFTau map
      mithep::PFTauArr              *taus_;           //array of taus
  };

}

#endif
