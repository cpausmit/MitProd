//--------------------------------------------------------------------------------------------------
// FillerPFJets
//
// Implementation of a filler to fill EDM particle flow jets into our mithep::PFJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFJETS_H
#define MITPROD_TREEFILLER_FILLERPFJETS_H

#include "MitAna/DataTree/interface/PFJetFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "DataFormats/BTauReco/interface/JetTag.h"

namespace mithep 
{
  class FillerPFJets : public BaseFiller
  {  
    public:
      FillerPFJets(const edm::ParameterSet &cfg, edm::ConsumesCollector&, const char *name, bool active=1);
      ~FillerPFJets();

      void              BookDataBlock(TreeWriter &tws);
      void 	        FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  		        
    private:	        
      bool              flavorMatchingActive_;                     //=true if flavor matching is done  
      bool              bTaggingActive_;                           //=true if bTagging info is filled
      bool              jetToVertexActive_;                        //=true if jet to vertex info is done
      bool              jetCorrectionsActive_;                     //=true if jet corrections are done
      bool              fastJetCorrectionsActive_;                 //=true if fastjet corrections are done
      edm::EDGetTokenT<reco::PFJetCollection> edmToken_;                                  //edm name of jets collection
      edm::EDGetTokenT<std::vector<double> > jetToVertexAlphaToken_;                     //edm name of jet to vertex alpha coll
      edm::EDGetTokenT<std::vector<double> > jetToVertexBetaToken_;                      //edm name of jet to vertex beta coll
      edm::EDGetTokenT<double> rhoToken_;                                  //edm name of Energy per unit area
      edm::EDGetTokenT<reco::JetMatchedPartonsCollection> flavorMatchingByReferenceToken_;            //source of flavor matching
      edm::EDGetTokenT<reco::JetTagCollection> jetProbabilityBJetTagsToken_;               //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> jetBProbabilityBJetTagsToken_;              //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> simpleSecondaryVertexHighEffBJetTagsToken_; //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> simpleSecondaryVertexHighPurBJetTagsToken_; //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> combinedSecondaryVertexBJetTagsToken_;      //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> combinedSecondaryVertexMVABJetTagsToken_;   //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> trackCountingHighEffBJetTagsToken_;         //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> trackCountingHighPurBJetTagsToken_;         //bjet algo discriminant
      std::string       mitName_;                                  //mit name of jets collection
      std::string       L2JetCorrectorName_;                       //label of the L2JetCorrection service
      std::string       L3JetCorrectorName_;                       //label of the L3JetCorrection service
      std::string       flavorMatchingDefinition_;                 //type of flavor matching
      std::string       pfCandMapName_;                            //name of imported PFCandidateMap
      const mithep::PFCandidateMap
	               *pfCandMap_;                                //map wrt PFCandidates
      std::string       jetMapName_;                               //name of exported PFJetMap
      mithep::PFJetMap *jetMap_;                                   //export map
      mithep::PFJetArr *jets_;                                     //array of Jets
  };
}
#endif
