//--------------------------------------------------------------------------------------------------
// $Id: FillerPFCandidates.h,v 1.7 2011/09/14 15:26:53 bendavid Exp $
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

      void                         BookDataBlock(TreeWriter &tws);
      void                         FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                         ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
  
    private:

      const mithep::Track *getMitTrack(mitedm::TrackPtr ptr, bool allowmissing) const;

      std::string                  edmName_;              //edm name of PFCandidates collection
      std::string                  mitName_;              //name of PFCandidate branch in BAMBU
      std::string                  globalTrackMapName_;   //name of imported map wrt global muons
      std::string                  staTrackMapName_;      //name of imported map wrt sta muons
      std::string                  staVtxTrackMapName_;   //name of imported map wrt sta vtx muons
      std::vector<std::string>     trackerTrackMapNames_;  //name of imported map wrt general tracks
      std::string                  gsfTrackMapName_;      //name of imported map wrt pf gsf tracks
      std::string                  muonMapName_;          //name of imported map wrt muons
      std::string                  conversionMapName_;    //name of imported map wrt conversions
      std::string                  pfCandMapName_;        //name of exported pf candidate map
      bool                         allowMissingTrackRef_; //allow missing track reference (needed for tau embedding samples)
      std::vector<const mithep::TrackMap*> trackerTrackMaps_; //maps wrt tracker tracks
      const mithep::TrackMap      *gsfTrackMap_;          //map wrt pf gsf tracks
      const mithep::MuonMap       *muonMap_;              //map wrt muons
      const mithep::ConversionMap *conversionMap_;        //map wrt conversions
      mithep::PFCandidateMap      *pfCandMap_;            //exported map
      mithep::PFCandidateArr      *pfCands_;              //array of PFCandidates
  };
}
#endif
