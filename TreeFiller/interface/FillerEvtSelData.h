//--------------------------------------------------------------------------------------------------
// $Id: FillerEvtSelData.h,v 1.2 2010/03/18 20:21:00 bendavid Exp $
//
// FillerEvtSelData
//
// Fill event selection data object.
//
// Authors: C.Loizides/L.Di Matteo
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLEREVTSELDATA_H
#define MITPROD_TREEFILLER_FILLEREVTSELDATA_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class EvtSelData;

  class FillerEvtSelData : public BaseFiller
  {
    public:
      FillerEvtSelData(const edm::ParameterSet &cfg, 
                       const char *name="EvtSelData", bool active=1);
      ~FillerEvtSelData();

      void                     BookDataBlock(TreeWriter &tws);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    protected:
      int                      GetMetFiltersWord (
                               Bool_t HBHENoiseFilter, Bool_t ECALDeadCellFilter,
                               Bool_t trackingFailureFilter, Bool_t EEBadScFilter,
                               Bool_t ECALaserCorrFilter, Bool_t tkOddManyStripClusFilter,
                               Bool_t tkOddTooManyStripClusFilter, Bool_t tkOddLogErrorTooManyClustersFilter,
                               Bool_t CSCTightHaloFilter, Bool_t CSCLooseHaloFilter);

    private:    
      std::string              mitName_;                        //mit branch name
      edm::InputTag            HBHENoiseFilterName_;            //name: input edm HBHENoiseFilter decision 
      edm::InputTag            ECALDeadCellFilterName_;         //name: input edm ECALDeadCellFilter decision 
      edm::InputTag            trackingFailureFilterName_;      //name: input edm trackingFailureFilter decision 
      edm::InputTag            EEBadScFilterName_;              //name: input edm EEBadScFilter decision 
      edm::InputTag            ECALaserCorrFilterName_;         //name: input edm ECALaserCorrFilter decision 
      edm::InputTag            tkManyStripClusName_;            //name: input edm trackingOddFilter decision [1]
      edm::InputTag            tkTooManyStripClusName_;         //name: input edm trackingOddFilter decision [2]
      edm::InputTag            tkLogErrorTooManyClustersName_;  //name: input edm trackingOddFilter decision [3]
      edm::InputTag            BeamHaloSummaryName_;            //name: input edm BeamHalo summary 
      EvtSelData               *evtSelData_;                    //event selection data object
  };
}
#endif
