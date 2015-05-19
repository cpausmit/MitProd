//--------------------------------------------------------------------------------------------------
// FillerPackedPFCandidates
//
// Implementation of a filler to fill EDM PackedCandidates into our mithep::PFCandidate data structure.
//
// Authors: Y.Iiyama
//--------------------------------------------------------------------------------------------------
#ifndef MITPROD_TREEFILLER_FILLERPACKEDPFCANDIDATES_H
#define MITPROD_TREEFILLER_FILLERPACKEDPFCANDIDATES_H

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "MitAna/DataTree/interface/PFCandidateFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerPackedPFCandidates : public BaseFiller
  {  
    public:
      FillerPackedPFCandidates(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerPackedPFCandidates();

      void                           BookDataBlock(TreeWriter &);
      void                           FillDataBlock(edm::Event const&, edm::EventSetup const&);

    private:
      edm::EDGetTokenT<pat::PackedCandidateCollection> edmToken_; //edm name of PFCandidates coll
      std::string                    mitName_;                  //name: PFCandidate branch in BAMBU
      std::string                    electronMapName_;          //name: impo. map wrt electrons
      std::string                    muonMapName_;              //name: impo. map wrt muons
      std::string                    photonMapName_;            //name: impo. map wrt photons
      std::string                    pfCandMapName_;            //name: expo. pf candidate map
      std::string                    pfNoPileupCandMapName_;    //name: expo. PFnoPU candidate map
      bool                           fillPfNoPileup_;

      const mithep::CandidateMap     *electronMap_;              //map wrt electrons
      const mithep::CandidateMap     *muonMap_;                  //map wrt muons
      const mithep::CandidateMap     *photonMap_;                //map wrt photons

      mithep::PFCandidateMap     *pfCandMap_;                //exported map
      mithep::PFCandidateMap     *pfNoPileupCandMap_;        //exported map for pf no pileup

      mithep::PFCandidateArr         *pfCands_;                  //array of PFCandidates
  };
}
#endif
