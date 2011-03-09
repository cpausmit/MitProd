//--------------------------------------------------------------------------------------------------
// $Id: FillerPFJets.h,v 1.9 2011/03/01 17:27:22 mzanetti Exp $
//
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

namespace mithep 
{
  class FillerPFJets : public BaseFiller
  {  
    public:
      FillerPFJets(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPFJets();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool            flavorMatchingActive_;            //=true if flavor matching is done  
      bool            bTaggingActive_;                  //=true if bTagging info is filled
      bool            jetToVertexActive_;               //=true if jet to vertex info is done
      bool            jetCorrectionsActive_;            //=true if jet corrections are done
      bool            fastJetCorrectionsActive_;        //=true if fastjet corrections are done
      std::string     edmName_;                         //edm name of jets collection
      std::string     mitName_;                         //mit name of jets collection
      std::string     jetToVertexAlphaName_;            //edm name of jet to vertex alpha coll
      std::string     jetToVertexBetaName_;             //edm name of jet to vertex beta coll
      edm::InputTag   rhoName_;                         //edm name of Energy per unit area
    //std::string     rhoName_;                         //edm name of Energy per unit area
      std::string     L2JetCorrectorName_;              //label of the L2JetCorrection service
      std::string     L3JetCorrectorName_;              //label of the L3JetCorrection service
      std::string     flavorMatchingByReferenceName_;   //source of flavor matching
      std::string     flavorMatchingDefinition_;        //type of flavor matching
      std::string     jetProbabilityBJetTagsName_;             //bjet algo discriminant
      std::string     jetBProbabilityBJetTagsName_;            //bjet algo discriminant
      std::string     simpleSecondaryVertexBJetTagsName_;      //bjet algo discriminant
      std::string     simpleSecondaryVertexHighEffBJetTagsName_;      //bjet algo discriminant
      std::string     simpleSecondaryVertexHighPurBJetTagsName_;      //bjet algo discriminant
      std::string     combinedSecondaryVertexBJetTagsName_;    //bjet algo discriminant
      std::string     combinedSecondaryVertexMVABJetTagsName_; //bjet algo discriminant
      std::string     ghostTrackBJetTagsName_;                 //bjet algo discriminant      
      std::string     trackCountingHighEffBJetTagsName_;       //bjet algo discriminant
      std::string     trackCountingHighPurBJetTagsName_;       //bjet algo discriminant
      std::string     softMuonBJetTagsName_;                   //bjet algo discriminant        
      std::string     softMuonByIP3dBJetTagsName_;             //bjet algo discriminant
      std::string     softMuonByPtBJetTagsName_;               //bjet algo discriminant
      std::string     softElectronByIP3dBJetTagsName_;         //bjet algo discriminant
      std::string     softElectronByPtBJetTagsName_;           //bjet algo discriminant
      std::string     pfCandMapName_;                   //name of imported PFCandidateMap
      std::string     jetMapName_;                      //name of exported PFJetMap
      const mithep::PFCandidateMap       *pfCandMap_;   //map wrt PFCandidates
      mithep::PFJetMap                   *jetMap_;      //export map
      mithep::PFJetArr                   *jets_;        //array of Jets
  };
}
#endif
