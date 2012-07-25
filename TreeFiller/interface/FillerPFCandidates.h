//--------------------------------------------------------------------------------------------------
// $Id: FillerPFCandidates.h,v 1.9 2012/03/11 23:11:55 pharris Exp $
//
// FillerPFCandidates
//
// Implementation of a filler to fill EDM PFCandidates into our mithep::PFCandidate data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFCANDIDATES_H
#define MITPROD_TREEFILLER_FILLERPFCANDIDATES_H

#include "MitAna/DataTree/interface/PFCandidateFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerPFCandidates : public BaseFiller
  {  
    public:
      FillerPFCandidates(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPFCandidates();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                           ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
  
    private:

      const mithep::Track *getMitTrack(mitedm::TrackPtr ptr, bool allowmissing) const;

      std::string                    edmName_;                  //edm name of PFCandidates coll
      std::string                    edmPfNoPileupName_;        //edm name of PFNoPileup  coll
      std::string                    mitName_;                  //name: PFCandidate branch in BAMBU
      std::string                    globalTrackMapName_;       //name: impo. map wrt global muons
      std::string                    staTrackMapName_;          //name: impo. map wrt sta muons
      std::string                    staVtxTrackMapName_;       //name: impo. map wrt sta vtx muons
      std::vector<std::string>       trackerTrackMapNames_;     //name: impo. map wrt general tracks
      std::string                    gsfTrackMapName_;          //name: impo. map wrt pf gsf tracks
      std::string                    muonMapName_;              //name: impo. map wrt muons
      std::string                    electronMapName_;          //name: impo. map wrt electrons
      std::string                    photonMapName_;            //name: impo. map wrt photons
      std::string                    barrelSuperClusterMapName_;      //name: impo. map wrt SC
      std::string                    endcapSuperClusterMapName_;      //name: impo. map wrt SC
      std::string                    pfElectronSuperClusterMapName_;  //name: impo. map wrt SC
      std::string                    conversionMapName_;        //name: impo. map wrt conversions
      std::string                    pfCandMapName_;            //name: expo. pf candidate map
      std::string                    pfNoPileupCandMapName_;    //name: expo. PFnoPU candidate map
      bool                           allowMissingTrackRef_;     //allow missing track ref (tau emb)
      bool                           fillPfNoPileup_;
      std::vector<const mithep::TrackMap*>
                                     trackerTrackMaps_;         //maps wrt tracker tracks
      const mithep::TrackMap        *gsfTrackMap_;              //map wrt pf gsf tracks
      const mithep::MuonMap         *muonMap_;                  //map wrt muons
      const mithep::ElectronMap     *electronMap_;              //map wrt electrons
      const mithep::PhotonMap       *photonMap_;                //map wrt photons
      const mithep::SuperClusterMap *barrelSuperClusterMap_;    //map wrt superclusters
      const mithep::SuperClusterMap *endcapSuperClusterMap_;    //map wrt superclusters
      const mithep::SuperClusterMap *pfElectronSuperClusterMap_;//map wrt superclusters
      const mithep::ConversionMap   *conversionMap_;            //map wrt conversions
      mithep::PFCandidateMap        *pfCandMap_;                //exported map
      mithep::PFCandidateMap        *pfNoPileupCandMap_;        //exported map for pf no pileup
      mithep::PFCandidateArr        *pfCands_;                  //array of PFCandidates
  };
}
#endif
