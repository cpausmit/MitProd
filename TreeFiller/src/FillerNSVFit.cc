// $Id: FillerNSVFit.cc,v 1.8 2011/04/26 12:14:24 mhchan Exp $

#include "MitAna/DataTree/interface/Names.h" //?
#include "MitProd/ObjectService/interface/ObjectService.h" //?

#include "MitAna/DataTree/interface/NSVFitCol.h"
#include "MitProd/TreeFiller/interface/FillerNSVFit.h"
#include "AnalysisDataFormats/TauAnalysis/interface/NSVfitEventHypothesisFwd.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerNSVFit::FillerNSVFit(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkNSVFitBrn)),
  hypotheses_(new mithep::NSVFitArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerNSVFit::~FillerNSVFit()
{
  // Destructor.

  delete hypotheses_;
}

//--------------------------------------------------------------------------------------------------
void FillerNSVFit::BookDataBlock(TreeWriter &tws)
{
  // Add NSVFit branch to tree.

  tws.AddBranch(mitName_,&hypotheses_);
  OS()->add<mithep::NSVFitArr>(hypotheses_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerNSVFit::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // Fill NSVFit results from edm into our collection.

  hypotheses_->Delete();

  // handle for the NSVFit info collection
  Handle<NSVfitEventHypothesisCollection> nSVfit;
  GetProduct(edmName_, nSVfit, event);
  
  for(NSVfitEventHypothesisCollection::const_iterator hyp = nSVfit->begin(); hyp != nSVfit->end(); ++hyp){
    for(edm::OwnVector<NSVfitResonanceHypothesis>::const_iterator res = hyp->resonances().begin(); res != hyp->resonances().end(); ++res){
      mithep::NSVFit *hypothesis = hypotheses_->Allocate();
      new (hypothesis) mithep::NSVFit();
      
      // fill info whether fit converged or not
      hypothesis->SetIsValid(res->isValidSolution());
      
      // fill masses
      hypothesis->SetMass(res->mass());	
      hypothesis->SetMassErrUp(res->massErrUp());	
      hypothesis->SetMassErrDown(res->massErrDown());	
      
      hypothesis->SetMassMean(res->mass_mean());	
      hypothesis->SetMassMedian(res->mass_median());	
      hypothesis->SetMassMaximum(res->mass_maximum());	
      hypothesis->SetMassMaxInterpol(res->mass_maxInterpol());	
      
      // fill p4 of input particles, which are the daughters of the resonance
      for(edm::OwnVector<NSVfitSingleParticleHypothesisBase>::const_iterator daughter = res->daughters().begin(); daughter != res->daughters().end(); ++daughter){
	hypothesis->AddDaughter(daughter->p4().pt(), daughter->p4().eta(), daughter->p4().phi(), daughter->p4().mass());
      }
    }
  }      
  hypotheses_->Trim();
}
