//--------------------------------------------------------------------------------------------------
// $Id: FilterConvPhi.cc,v 1.1 2010/01/18 14:37:18 bendavid Exp $
//
// FilterConvPhi
//
// Filter to select events with simulation-level conversions with two matched reconstructed tracks
// in a particular phi range.
// Requires TrackingParticles in the event.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/MCParticle.h"
#include "MitAna/DataTree/interface/TrackCol.h"
#include "MitAna/DataTree/interface/MCParticleCol.h"
#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include <TMath.h>

namespace mitedm
{
  class FilterConvPhi : public edm::EDFilter {
  public:
    explicit FilterConvPhi(const edm::ParameterSet &ps);
    ~FilterConvPhi() {}
    
  protected:
    virtual bool filter (edm::Event &iEvent, const edm::EventSetup &iSetup);
    void beginJob(const edm::EventSetup &es);
    bool FailedSimCuts(const mithep::MCParticle* p) const;
    const mithep::Track *MatchedTrack(const mithep::MCParticle* p) const;
    double minPhi_;
    double maxPhi_;
    const mithep::TrackArr *tracks_;
    const mithep::MCParticleArr *mcParts_;
  };
}

using namespace mitedm;
using namespace mithep;
using namespace edm;
using namespace std;

//--------------------------------------------------------------------------------------------------
FilterConvPhi::FilterConvPhi(const edm::ParameterSet& iConfig)
  : minPhi_(iConfig.getUntrackedParameter<double>("minPhi",-TMath::Pi())),
    maxPhi_(iConfig.getUntrackedParameter<double>("maxPhi",-3.1))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------

void FilterConvPhi::beginJob(const edm::EventSetup &event)
{
  tracks_ = FillMitTree::os()->get<TrackArr>("Tracks");
  mcParts_ = FillMitTree::os()->get<MCParticleArr>("MCParticles");
}

//--------------------------------------------------------------------------------------------------
bool FilterConvPhi::filter( edm::Event &iEvent, const edm::EventSetup &iSetup)
{
  // Filter events based on specified bunch crossings.
  
  for (UInt_t i=0; i<mcParts_->GetEntries(); ++i) {
    const MCParticle *p = mcParts_->At(i);
    if (!FailedSimCuts(p))
      return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------
const Track *FilterConvPhi::MatchedTrack(const MCParticle* p) const
{
  for (UInt_t i=0; i<tracks_->GetEntries(); ++i) {
    const Track *t = tracks_->At(i);
    const MCParticle *mc = t->MCPart();
    if (mc) {
      Int_t pdg = mc->PdgId();
      while (mc && mc->PdgId()==pdg) {
        if (mc==p)
          return t;
        mc = mc->Mother();
      }
    }
  }
  
  return 0;
  
}

bool FilterConvPhi::FailedSimCuts(const MCParticle* p) const {        
        
        if (p->PdgId()!=22)
          return true;
        
        if (p->Phi()<minPhi_ || p->Phi()>maxPhi_)
          return true;
        
        Int_t nEPlus = 0;
        Int_t nEMinus = 0;
        for (UInt_t j=0; j<p->NDaughters(); j++) {
          const MCParticle* pDaughter = p->Daughter(j);
          if (pDaughter->PdgId()==11)
                  nEPlus++;
          if (pDaughter->PdgId()==-11)
                  nEMinus++;
          if (TMath::Abs(pDaughter->PdgId())==11) {
            if (!MatchedTrack(pDaughter))
              return true;
            
          }
        }
        
        if (nEPlus!=1 || nEMinus!=1)
          return true;
        
        return false;
}


//define this as a plug-in
DEFINE_FWK_MODULE(FilterConvPhi);
