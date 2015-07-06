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
    FillerPackedPFCandidates(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* = "PackedPFCandidates", bool = true);
    ~FillerPackedPFCandidates();

    void BookDataBlock(TreeWriter &) override;
    void PrepareLinks() override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

  private:
    edm::EDGetTokenT<pat::PackedCandidateCollection> edmToken_; //edm name of PFCandidates coll
    std::string                 mitName_;                  //name: PFCandidate branch in BAMBU
    std::string                 pfCandMapName_;            //name: expo. pf candidate map
    std::string                 pfNoPileupCandMapName_;    //name: expo. PFnoPU candidate map
    bool                        fillPfNoPileup_;
    std::string                 electronMapName_;
    std::string                 muonMapName_;
    std::string                 photonMapName_;
    std::string                 trackMapName_;

    mithep::PFCandidateMap*     pfCandMap_;                //exported map
    mithep::PFCandidateMap*     pfNoPileupCandMap_;        //exported map for pf no pileup

    mithep::PFCandidateArr*     pfCands_;                  //array of PFCandidates

    mithep::CandidateMap const* electronMap_;
    mithep::CandidateMap const* muonMap_;
    mithep::CandidateMap const* photonMap_;
    mithep::CandidateMap const* trackMap_;
  };
}
#endif
