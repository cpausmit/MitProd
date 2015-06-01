//--------------------------------------------------------------------------------------------------
//
// Implementation of a filler creating distance of closes approach between two opposite flavored leptons (including taus)
//
// Authors: P. Harrs 
//--------------------------------------------------------------------------------------------------
#ifndef MITPROD_TREEFILLER_FILLERDCASIG_H
#define MITPROD_TREEFILLER_FILLERDCASIG_H

#include "MitAna/DataTree/interface/DCASigFwd.h"
#include "MitAna/DataTree/interface/DCASig.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TauReco/interface/BaseTau.h"
#include "DataFormats/Common/interface/View.h"

namespace mithep 
{
  class FillerDCASig : public BaseFiller
  {  
    public:
    FillerDCASig(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
    ~FillerDCASig();

      void                                     BookDataBlock(TreeWriter &tws);
      void                                     FillDataBlock(const edm::Event &e, 
                                                             const edm::EventSetup &es);
      void calculateDCA(Double_t &iDCA3D    ,Double_t &iDCA3DE    ,Double_t &iDCA2D    ,Double_t &iDCA2DE,
			Double_t &iDCARPhi3D,Double_t &iDCARPhi3DE,Double_t &iDCARPhi2D,Double_t &iDCARPhi2DE,
			const reco::Track *iTrack1,const reco::Track *iTrack2,DCASig::EDCAType iType) ;

      typedef edm::View<reco::GsfElectron> GsfElectronView;
      typedef edm::View<reco::Muon> MuonView;
      typedef edm::View<reco::BaseTau> BaseTauView;

      enum TauType {
        kPFTau,
        kPATTau,
        nTauTypes
      };

    private:
      mithep::Particle                        *GetMitParticle(edm::Ptr<reco::Track> ptr) const;
    
      edm::EDGetTokenT<GsfElectronView> edmElectronToken_;     //name of edm electrons
      edm::EDGetTokenT<MuonView> edmMuonToken_;         //name of edm muons
      edm::EDGetTokenT<BaseTauView> edmTauToken_;          //name of edm Taus
      TauType                                  edmTauType_;
      std::string                              mitName_;             //mit name of Conversions
      std::string                              electronMapName_;     //name of imported electrons map
      std::string                              muonMapName_;         //name of imported muons map
      std::string                              tauMapName_;          //name of imported pftau map
      const mithep::ElectronMap               *electronMap_;         //map wrt electrons
      const mithep::MuonMap                   *muonMap_;             //map wrt muons
      const mithep::PFTauMap                  *tauMap_;              //map wrt pftaus
      Double_t                                 electronPtMin_;       //Minimum electron pT for wich to calc DCA
      Double_t                                 muonPtMin_;           //Minimum muon pT for which to calc DCA
      Double_t                                 tauPtMin_;            //Minimum tau pT for which to calc DCA
      Bool_t                                   checkDCARef_;         //Debug Output
      mithep::DCASigArr                       *DCASigs_;             //array of DCASignificance
      const  TransientTrackBuilder            *transientTrackBuilder_;
  };
}
#endif
