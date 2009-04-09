// $Id: FillerCaloTaus.cc,v 1.2 2009/03/22 10:00:46 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerCaloTaus.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/TauReco/interface/CaloTau.h"
#include "DataFormats/TauReco/interface/CaloTauFwd.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloTaus::FillerCaloTaus(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","recoCaloTaus:iterativeCone5CaloTaus")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloTauBrn)), 
  trackMapName_(Conf().getUntrackedParameter<string>("trackMapName","TracksMapName")), 
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","JetMapName")), 
  barrelBCMapName_(Conf().getUntrackedParameter<string>("barrelBCMapName","")), 
  endcapBCMapName_(Conf().getUntrackedParameter<string>("endcapBCMapName","")), 
  trackMap_(0),
  jetMap_(0),
  barrelBCMap_(0),
  endcapBCMap_(0),
  taus_(new mithep::CaloTauArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloTaus::~FillerCaloTaus()
{
  // Destructor.

  delete taus_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloTaus::BookDataBlock(TreeWriter &tws)
{
  // Add taus branch to tree.

  tws.AddBranch(mitName_,&taus_);
  OS()->add<mithep::CaloTauArr>(taus_,mitName_);

  if (!trackMapName_.empty()) {
    trackMap_ = OS()->get<TrackMap>(trackMapName_);
    if (trackMap_)
      AddBranchDep(mitName_, trackMap_->GetBrName());
  }

  if (!jetMapName_.empty()) {
    jetMap_ = OS()->get<CaloJetMap>(jetMapName_);
    if (jetMap_)
      AddBranchDep(mitName_, jetMap_->GetBrName());
  }

  if (!barrelBCMapName_.empty()) {
    barrelBCMap_ = OS()->get<BasicClusterMap>(barrelBCMapName_);
    if (barrelBCMap_)
      AddBranchDep(mitName_, barrelBCMap_->GetBrName());
  }

  if (!endcapBCMapName_.empty()) {
    endcapBCMap_ = OS()->get<BasicClusterMap>(endcapBCMapName_);
    if (endcapBCMap_)
      AddBranchDep(mitName_, endcapBCMap_->GetBrName());
  }
}

//--------------------------------------------------------------------------------------------------
void FillerCaloTaus::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill taus from edm collection into our collection.

  taus_->Delete();

  // handle for the tau collection
  Handle<reco::CaloTauCollection> hTauProduct;
  GetProduct(edmName_, hTauProduct, event);
  
  const reco::CaloTauCollection inTaus = *(hTauProduct.product());  
  // loop through all taus
  for (reco::CaloTauCollection::const_iterator inTau = inTaus.begin(); 
       inTau != inTaus.end(); ++inTau) {
    
    reco::CaloTauRef tauRef(hTauProduct, inTau - inTaus.begin());
    
    mithep::CaloTau *tau = taus_->Allocate();
    new (tau) mithep::CaloTau(inTau->px(),
                              inTau->py(),
                              inTau->pz(),
                              inTau->energy());

    tau->SetMomAlt(inTau->alternatLorentzVect().x(),
                   inTau->alternatLorentzVect().y(),
                   inTau->alternatLorentzVect().z(),
                   inTau->alternatLorentzVect().e());

    // fill calotau-specific quantities
    tau->SetLeadTrackSignD0Sig(inTau->leadTracksignedSipt());
    tau->SetLeadTrack3x3HCalEt(inTau->leadTrackHCAL3x3hitsEtSum());
    tau->SetLeadTrack3x3HCalDEta(inTau->leadTrackHCAL3x3hottesthitDEta());
    tau->SetSignalTracksMass(inTau->signalTracksInvariantMass());
    tau->SetTracksMass(inTau->TracksInvariantMass());
    tau->SetIsoTrackPtSum(inTau->isolationTracksPtSum());
    tau->SetIsoECalEtSum(inTau->isolationECALhitsEtSum());
    tau->SetMaxHCalHitEt(inTau->maximumHCALhitEt());

    // add track references
    if (trackMap_) {
      // lead track reference
      if (inTau->leadTrack().isNonnull())
        tau->SetLeadTrack(trackMap_->GetMit(refToPtr(inTau->leadTrack())));

      // signal tracks
      for (uint i=0; i<inTau->signalTracks().size(); ++i) {
        const Track *signalTrack = trackMap_->GetMit(refToPtr(inTau->signalTracks().at(i)));
        tau->AddSignalTrack(signalTrack);
      }

      // isolation tracks
      for (uint i=0; i<inTau->isolationTracks().size(); ++i) {
        const Track *isoTrack = trackMap_->GetMit(refToPtr(inTau->isolationTracks().at(i)));
        tau->AddIsoTrack(isoTrack);
      }
    }

    const reco::CaloTauTagInfo *tagInfo = inTau->caloTauTagInfoRef().get();

    // add source calojet reference
    if (jetMap_) {
      tau->SetCaloJet(jetMap_->GetMit(refToPtr(tagInfo->calojetRef())));
    }

    // add neutral basic cluster references
    if (barrelBCMap_ && endcapBCMap_) { 
      for (uint i=0; i<tagInfo->neutralECALBasicClusters().size(); ++i) {
        reco::BasicClusterRef clusterRef = tagInfo->neutralECALBasicClusters().at(i);
        if ( barrelBCMap_->HasMit(clusterRef) )
          tau->AddNeutralBC(barrelBCMap_->GetMit(clusterRef));
        else if ( endcapBCMap_->HasMit(clusterRef) )
          tau->AddNeutralBC(endcapBCMap_->GetMit(clusterRef));
        else throw edm::Exception(edm::errors::Configuration, "FillerCaloTaus:FillDataBlock()\n")
             << "Error! Basic Cluster reference in unmapped collection " << edmName_ << endl;
      }
    }
  }      
  taus_->Trim();
}
