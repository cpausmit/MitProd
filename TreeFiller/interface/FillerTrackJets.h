//--------------------------------------------------------------------------------------------------
// $Id: FillerTrackJets.h,v 1.1 2010/02/24 17:38:25 bendavid Exp $
//
// FillerTrackJets
//
// Implementation of a filler to fill EDM track jets into our mithep::TrackJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERTRACKJETS_H
#define MITPROD_TREEFILLER_FILLERTRACKJETS_H

#include "MitAna/DataTree/interface/PFJetFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/JetReco/interface/TrackJetCollection.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "DataFormats/BTauReco/interface/JetTag.h"

namespace mithep 
{
  class FillerTrackJets : public BaseFiller
  {  
    public:
      FillerTrackJets(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerTrackJets();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool            flavorMatchingActive_;            //=true if flavor matching is done  
      bool            bTaggingActive_;                  //=true if bTagging info is filled
      bool            jetToVertexActive_;               //=true if jet to vertex info is done
      bool            jetCorrectionsActive_;            //=true if jet corrections are done
      edm::EDGetTokenT<reco::TrackJetCollection> edmToken_;                         //edm name of jets collection
      edm::EDGetTokenT<std::vector<double> > jetToVertexAlphaToken_;            //edm name of jet to vertex alpha coll
      edm::EDGetTokenT<std::vector<double> > jetToVertexBetaToken_;             //edm name of jet to vertex beta coll
      edm::EDGetTokenT<reco::JetMatchedPartonsCollection> flavorMatchingByReferenceToken_;   //source of flavor matching
      edm::EDGetTokenT<reco::JetTagCollection> jetProbabilityBJetTagsToken_;             //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> jetBProbabilityBJetTagsToken_;            //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> simpleSecondaryVertexBJetTagsToken_;      //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> combinedSecondaryVertexBJetTagsToken_;    //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> combinedSecondaryVertexMVABJetTagsToken_; //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> trackCountingHighEffBJetTagsToken_;       //bjet algo discriminant
      edm::EDGetTokenT<reco::JetTagCollection> trackCountingHighPurBJetTagsToken_;       //bjet algo discriminant

      std::string     L2JetCorrectorName_;              //label of the L2JetCorrection service
      std::string     L3JetCorrectorName_;              //label of the L3JetCorrection service
      std::string     flavorMatchingDefinition_;        //type of flavor matching
      std::string     mitName_;                         //mit name of jets collection
      std::string     trackMapName_;                   //name of imported TrackMap
      std::string     vertexMapName_;                   //name map wrt primary vertexes
      std::string     jetMapName_;                      //name of exported PFJetMap
      const mithep::TrackMap             *trackMap_;   //map wrt PFCandidates
      const mithep::VertexMap            *vertexMap_;        //map wrt primary vertexes
      mithep::TrackJetMap                *jetMap_;      //export map
      mithep::TrackJetArr                *jets_;        //array of Jets
  };
}
#endif
