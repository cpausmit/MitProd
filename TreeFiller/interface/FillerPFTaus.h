//--------------------------------------------------------------------------------------------------
// $Id: FillerPFTaus.h,v 1.7 2011/04/29 14:18:23 mhchan Exp $
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
      std::string                    trackMapName_;   //name of imported TrackMap
      std::string                    jetMapName_;     //name of imported PFJetMap
      std::string                    pfCandMapName_;  //name of imported PFCandidateMap
      const mithep::TrackMap        *trackMap_;       //map wrt Tracks
      const mithep::PFJetMap        *jetMap_;         //map wrt pfjets
      const mithep::PFCandidateMap  *pfCandMap_;      //map wrt pf candidates
      mithep::PFTauArr              *taus_;           //array of taus
          
      template <typename C>
      edm::Ptr<typename C::value_type> refToPtrHack(edm::Ref<C, typename C::value_type, edm::refhelper::FindUsingAdvance<C, typename C::value_type> > const& ref) {
        typedef typename C::value_type T;
        return edm::Ptr<T>(ref.id(), ref.get(), ref.key());
      }      

  };
}
#endif
