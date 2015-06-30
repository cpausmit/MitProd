//--------------------------------------------------------------------------------------------------
// FillerPFTaus
//
// Implementation of a filler to fill EDM taus into our mithep::PFTau data structure.
// Source tau class can be either reco::PFTau or pat::Tau, which share only the base class
// (unlike other leptons where standard reco:: object is the base class of the pat:: object).
//
// Authors: C.Loizides, J.Bendavid, C.Paus, Y.Iiyama
//--------------------------------------------------------------------------------------------------

// reverse header protection - this file should be included only by FillerPFTaus.cc
#ifdef MITPROD_TREEFILLER_FILLERPFTAUS

#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"

#include "MitAna/DataTree/interface/PFTauCol.h"

namespace mithep {

  template<class TAU>
  class FillerPFTaus : public BaseFiller {
  public:
    FillerPFTaus(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerPFTaus();

    void BookDataBlock(TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;

    typedef std::vector<TAU> TauCollection;

  private:
    void retrieveDiscriminators(reco::PFTauDiscriminator const* [mithep::PFTau::nDiscriminators], edm::Event const&) const;
    void setPFSpecific(mithep::PFTau&, TAU const&) const;
    void setPFTauDiscriminators(mithep::PFTau&, reco::PFTauDiscriminator const* [mithep::PFTau::nDiscriminators], edm::Ref<TauCollection> const&) const;
    void setPFJetRef(mithep::PFTau&, TAU const&) const;
    void setPFCandRefs(mithep::PFTau&, TAU const&) const;

    bool hpsActive_; //=true if HPS discriminants are filled
    edm::EDGetTokenT<TauCollection> edmToken_; //edm name of jets collection
    edm::EDGetTokenT<reco::PFTauDiscriminator> hpsTokens_[mithep::PFTau::nDiscriminators];
    std::string hpsNames_[mithep::PFTau::nDiscriminators]; //edm name of jets collection
    std::string mitName_; //mit name of jets collection
    std::vector<std::string> trackMapNames_; //name of imported TrackMap
    std::string jetMapName_; //name of imported PFJetMap
    std::string pfCandMapName_; //name of imported PFCandidateMap
    std::string tauMapName_; //name of exported PFTau Map
    bool allowMissingTrackRef_; //allow missing track reference
                                //-->needed for tau embedding samples
    std::vector<mithep::TrackMap const*> trackMaps_; //map wrt Tracks
    mithep::PFJetMap const* jetMap_; //map wrt pfjets
    mithep::PFCandidateMap const* pfCandMap_; //map wrt pf candidates
    mithep::PFTauMap* tauMap_; //exported PFTau map
    mithep::PFTauArr* taus_; //array of taus
  };

}

#endif
