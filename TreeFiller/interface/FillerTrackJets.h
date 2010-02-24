//--------------------------------------------------------------------------------------------------
// $Id: FillerTrackJets.h,v 1.6 2009/09/25 08:42:50 loizides Exp $
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

namespace mithep 
{
  class FillerTrackJets : public BaseFiller
  {  
    public:
      FillerTrackJets(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerTrackJets();

      void            BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool            flavorMatchingActive_;            //=true if flavor matching is done  
      bool            bTaggingActive_;                  //=true if bTagging info is filled
      bool            jetToVertexActive_;               //=true if jet to vertex info is done
      bool            jetCorrectionsActive_;            //=true if jet corrections are done
      std::string     edmName_;                         //edm name of jets collection
      std::string     mitName_;                         //mit name of jets collection
      std::string     jetToVertexAlphaName_;            //edm name of jet to vertex alpha coll
      std::string     jetToVertexBetaName_;             //edm name of jet to vertex beta coll
      std::string     L2JetCorrectorName_;              //label of the L2JetCorrection service
      std::string     L3JetCorrectorName_;              //label of the L3JetCorrection service
      std::string     flavorMatchingByReferenceName_;   //source of flavor matching
      std::string     flavorMatchingDefinition_;        //type of flavor matching
      std::string     jetProbabilityBJetTagsName_;             //bjet algo discriminant
      std::string     jetBProbabilityBJetTagsName_;            //bjet algo discriminant
      std::string     simpleSecondaryVertexBJetTagsName_;      //bjet algo discriminant
      std::string     combinedSecondaryVertexBJetTagsName_;    //bjet algo discriminant
      std::string     combinedSecondaryVertexMVABJetTagsName_; //bjet algo discriminant
      std::string     trackCountingHighEffBJetTagsName_;       //bjet algo discriminant
      std::string     trackCountingHighPurBJetTagsName_;       //bjet algo discriminant
      std::string     softMuonBJetTagsName_;                   //bjet algo discriminant        
      std::string     softMuonByIP3dBJetTagsName_;             //bjet algo discriminant
      std::string     softMuonByPtBJetTagsName_;               //bjet algo discriminant
      std::string     softElectronByIP3dBJetTagsName_;         //bjet algo discriminant
      std::string     softElectronByPtBJetTagsName_;           //bjet algo discriminant
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
