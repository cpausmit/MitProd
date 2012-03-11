//--------------------------------------------------------------------------------------------------
// $Id: FillerPFTaus.h,v 1.11 2011/10/10 20:57:40 bendavid Exp $
//
// FillerPFTaus
//
// Implementation of a filler to fill EDM pf taus into our mithep::PFTau data structure.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFTAUS_H
#define MITPROD_TREEFILLER_FILLERPFTAUS_H

#include "MitAna/DataTree/interface/PFTauFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerPFTaus : public BaseFiller
  {  
    public:
      FillerPFTaus(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPFTaus();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      //template <typename C> edm::Ptr<typename C::value_type> refToPtrHack(edm::Ref<typename C, edm::refhelper::FindUsingAdvance<C, typename C::value_type> > const &ref)  { return edm::Ptr<typename C::value_type>(ref.id(), ref.get(), ref.key()); }
      
      const mithep::Track *getMitTrack(mitedm::TrackPtr ptr, bool allowmissing) const;


      bool                           hpsActive_;      //=true if HPS discriminants are filled
      std::string                    edmName_;        //edm name of jets collection
      std::string                    mitName_;        //mit name of jets collection
      std::string                    discriminationByLooseElectronRejectionName_; // HPS discriminant
      std::string                    discriminationByMediumElectronRejectionName_; // HPS discriminant
      std::string                    discriminationByTightElectronRejectionName_; // HPS discriminant
      std::string                    discriminationByLooseMuonRejectionName_; // HPS discriminant
      std::string                    discriminationByTightMuonRejectionName_; // HPS discriminant
      std::string                    discriminationByDecayModeFindingName_; // HPS discriminant
      std::string                    discriminationByVLooseIsolationName_;   // HPS discriminant
      std::string                    discriminationByLooseIsolationName_;   // HPS discriminant
      std::string                    discriminationByMediumIsolationName_;  // HPS discriminant
      std::string                    discriminationByTightIsolationName_;   // HPS discriminant
      std::string                    discriminationByVLooseCombinedIsolationDBSumPtCorrName_; // HPS discriminant
      std::string                    discriminationByLooseCombinedIsolationDBSumPtCorrName_;  // HPS discriminant
      std::string                    discriminationByMediumCombinedIsolationDBSumPtCorrName_; // HPS discriminant
      std::string                    discriminationByTightCombinedIsolationDBSumPtCorrName_;  // HPS discriminant
      std::vector<std::string>       trackMapNames_;   //name of imported TrackMap
      std::string                    jetMapName_;     //name of imported PFJetMap
      std::string                    pfCandMapName_;  //name of imported PFCandidateMap
      std::string                    tauMapName_;     //name of exported PFTau Map
      bool                           allowMissingTrackRef_; //allow missing track reference (needed for tau embedding samples)
      std::vector<const mithep::TrackMap*> trackMaps_;       //map wrt Tracks
      const mithep::PFJetMap        *jetMap_;         //map wrt pfjets
      const mithep::PFCandidateMap  *pfCandMap_;      //map wrt pf candidates
      mithep::PFTauMap              *tauMap_;         //exported PFTau map
      mithep::PFTauArr              *taus_;           //array of taus
          
      template <typename C>
      edm::Ptr<typename C::value_type> refToPtrHack(edm::Ref<C, typename C::value_type, edm::refhelper::FindUsingAdvance<C, typename C::value_type> > const& ref) {
        typedef typename C::value_type T;
        return edm::Ptr<T>(ref.id(), ref.get(), ref.key());
      }      

  };
}
#endif
