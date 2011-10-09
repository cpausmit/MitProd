// $Id: FillerNSVFit.cc,v 1.2 2011/06/16 11:40:20 rwolf Exp $

#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include "MitAna/DataTree/interface/NSVFitCol.h"
#include "MitProd/TreeFiller/interface/FillerNSVFit.h"

// #include "AnalysisDataFormats/TauAnalysis/interface/NSVfitEventHypothesisBase.h"
// #include "AnalysisDataFormats/TauAnalysis/interface/NSVfitEventHypothesisBaseFwd.h"
// #include "AnalysisDataFormats/TauAnalysis/interface/NSVfitResonanceHypothesis.h"
// #include "AnalysisDataFormats/TauAnalysis/interface/NSVfitSingleParticleHypothesis.h"

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
//   Handle<edm::View<NSVfitEventHypothesisBase> > nSVfit;
//   GetProduct(edmName_, nSVfit, event);
  
/*  for(edm::View<NSVfitEventHypothesisBase>::const_iterator evtHyp = nSVfit->begin(); evtHyp != nSVfit->end(); ++evtHyp){
    for(unsigned int ires=0; ires<evtHyp->numResonances(); ++ires){
      const NSVfitResonanceHypothesis* resHyp = dynamic_cast<const NSVfitResonanceHypothesis*> (evtHyp->resonance(ires));
      if( resHyp ){
	// fill the MIT structures
	mithep::NSVFit* hypothesis = hypotheses_->Allocate();
	new (hypothesis) mithep::NSVFit();
	
	// fill info whether fit converged or not
	hypothesis->SetIsValid(resHyp->isValidSolution());
	
	// fill masses
	hypothesis->SetMass(resHyp->mass());	
	hypothesis->SetMassErrUp(resHyp->massErrUp());	
	hypothesis->SetMassErrDown(resHyp->massErrDown());	
	
	hypothesis->SetMassMean(resHyp->mass_mean());	
	hypothesis->SetMassMedian(resHyp->mass_median());	
	hypothesis->SetMassMaximum(resHyp->mass_maximum());	
	hypothesis->SetMassMaxInterpol(resHyp->mass_maxInterpol());	
	
	// fill p4 of input particles, which are the daughters of the resonance
	for(unsigned int idaug=0; idaug<resHyp->numDaughters(); ++idaug){
	  const NSVfitSingleParticleHypothesis* part = dynamic_cast<const NSVfitSingleParticleHypothesis*> (resHyp->daughter(idaug));
	  if( part ){
	    hypothesis->AddDaughter(part->p4().pt(), part->p4().eta(), part->p4().phi(), part->p4().mass());
	  }
	}
      }
    }
  }   */   
  hypotheses_->Trim();
}
