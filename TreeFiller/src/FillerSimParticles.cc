// $Id: FillerSimParticles.cc,v 1.5 2008/07/07 13:29:05 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerSimParticles.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "MitAna/DataTree/interface/Particle.h"
#include "MitAna/DataTree/interface/SimParticle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerSimParticles::FillerSimParticles(const ParameterSet &cfg, bool active, const GenParticleMap *genMap) : 
  BaseFiller(cfg, "SimParticles", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","mergedtruth:MergedTrackTruth")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkSimPartBrn)),
  genMap_(genMap),
  simParticles_(new mithep::SimParticleArr), 
  simMap_(new mithep::SimParticleMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerSimParticles::~FillerSimParticles()
{
  // Destructor.

  delete simParticles_;
  delete simMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerSimParticles::BookDataBlock(TreeWriter &tws)
{
  // Add simparticles branch to tree.

  tws.AddBranch(mitName_.c_str(),&simParticles_);
}

//--------------------------------------------------------------------------------------------------
void FillerSimParticles::FillDataBlock(const edm::Event      &event, 
                                       const edm::EventSetup &setup)
{
  // Fill simParticles data.

  simParticles_->Reset();
  simMap_->Reset();
  
  try {
    event.getByLabel(edm::InputTag(edmName_),trackingParticleProduct);
  } catch (cms::Exception &ex) {
    edm::LogError("FillerSimParticles") << "Error! Cannot get collection with label " 
                                        << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerSimParticles:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
  
  const TrackingParticleCollection trackingParticles = *(trackingParticleProduct.product());  
  
  // loop through all simParticles
  for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin(); 
       iM != trackingParticles.end(); ++iM) {

      TrackingParticleRef theRef(trackingParticleProduct, iM-trackingParticles.begin());

      mithep::SimParticle *outSimParticle = simParticles_->Allocate();
      new (outSimParticle) mithep::SimParticle(iM->px(),iM->py(),iM->pz(),iM->energy(),iM->pdgId());
      simMap_->Add(theRef, outSimParticle);
      if (genMap_ && iM->genParticle().size()) {
        const HepMC::GenParticle* mcPart = iM->genParticle_begin()->get();
        outSimParticle->SetGenParticle(genMap_->GetMit(mcPart->barcode()));
      }
  }
  
  simParticles_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerSimParticles::ResolveLinks(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Resolve links among simParticles.

  const TrackingParticleCollection trackingParticles = *(trackingParticleProduct.product());  

  // loop through all simParticles
  for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin(); 
       iM != trackingParticles.end(); ++iM) {
  
    if (iM->decayVertices().size() <= 0) 
      continue;

    TrackingParticleRef theRef(trackingParticleProduct, iM-trackingParticles.begin());
    mithep::SimParticle* simParent = simMap_->GetMit(theRef);
    for (TrackingVertexRefVector::iterator v= iM->decayVertices().begin(); 
         v != iM->decayVertices().end(); ++v) {
      for (TrackingParticleRefVector::iterator pd = v->get()->daughterTracks().begin(); 
           pd != v->get()->daughterTracks().end(); ++pd) {
        mithep::SimParticle* simDaughter = simMap_->GetMit(*pd);
        simParent->AddDaughter(simDaughter);
        simDaughter->SetMother(simParent);
      }
      if (v == iM->decayVertices().end()-1) {
        simParent->SetVertex(v->get()->position().x(),
                             v->get()->position().y(),
                             v->get()->position().z());
      }
    }
  }     
}
