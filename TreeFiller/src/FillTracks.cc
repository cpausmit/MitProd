// $Id: FillTracks.cc,v 1.1 2008/06/05 16:07:11 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillTracks.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "MitAna/DataTree/interface/Names.h"

#include "TLorentzVector.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillTracks::FillTracks(const edm::ParameterSet &iConfig)
  : trackSource_(iConfig.getUntrackedParameter<string>("trackSource" , "generalTracks")),
    trackBranch_(iConfig.getUntrackedParameter<string>("trackBrname", Names::gkTrackBrn))
{
   tracks_ = new mithep::Vector<mithep::Track>();
}

//-------------------------------------------------------------------------------------------------
FillTracks::~FillTracks()
{
  cout << " Filltracks done " <<endl;
}

//-------------------------------------------------------------------------------------------------
void FillTracks::analyze(const edm::Event &theEvent, 
			 const edm::EventSetup &iSetup)
{
  tracks_->Reset();

  Handle<reco::TrackCollection> theTrackProduct;
  try {
    theEvent.getByLabel(trackSource_, theTrackProduct);
  } catch (cms::Exception& ex) {
    edm::LogError("FillTracks") << "Error! Can not get collection with label " 
                                  << trackSource_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillTracks:analyze()\n")
      << "Error! Can not get collection with label " << trackSource_ << endl;
  }

  const reco::TrackCollection Tracks = *(theTrackProduct.product());  

  int nTracks = 0;
  for (reco::TrackCollection::const_iterator inTrack = 
         Tracks.begin(); inTrack != Tracks.end(); ++inTrack) {
    
    mithep::Track* outTrack = new mithep::Track(inTrack->phi(),inTrack->d0(),inTrack->pt(),
						inTrack->dz(),inTrack->theta());
    
    outTrack->SetErrors(inTrack->phiError(),inTrack->d0Error(),inTrack->ptError(),
			inTrack->dzError(),inTrack->thetaError());
    outTrack->SetCharge(inTrack->charge());
    
    
    tracks_->Add(outTrack);
    nTracks++;
  }

}

//-------------------------------------------------------------------------------------------------
void FillTracks::beginJob(edm::EventSetup const &iEvent)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if (!tws) {
    throw edm::Exception(edm::errors::Configuration, "FillTracks::beginJob()\n")
      << "Could not get pointer to Tree with name " << tws->GetName() << "\n";
    return;
  }

  tws->AddBranch(trackBranch_.c_str(),&tracks_);
}

//-------------------------------------------------------------------------------------------------
void FillTracks::endJob()
{
  edm::LogInfo("FillMuons::endJob") << "Ending Job" << endl;
}
