//--------------------------------------------------------------------------------------------------
// $Id: FillerEmbedWeight.h,v 1.3 2013/05/06 18:27:45 pharris Exp $
//
// FillerMCEventInfo
//
// Implementation of a filler that stores MC related quantities, like the pdf info, in the
// bambu records.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLEREMBEDWEIGHT_H
#define MITPROD_TREEFILLER_FILLEREMBEDWEIGHT_H
 
#include "MitAna/DataTree/interface/EmbedWeightFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "SimDataFormats/GeneratorProducts/interface/GenFilterInfo.h"

namespace mithep 
{
  class FillerEmbedWeight : public BaseFiller
  {
    public:
      FillerEmbedWeight(const edm::ParameterSet &cfg, 
                        edm::ConsumesCollector&,
                        ObjectService*,
                        const char *name="EmbedWeight", bool active=1);
      ~FillerEmbedWeight();

      void                     BookDataBlock(TreeWriter &tws);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      edm::EDGetTokenT<double> genWeightToken_;
      edm::EDGetTokenT<GenFilterInfo> genInfoToken_;
      edm::EDGetTokenT<double> spinnerWeightToken_;
      edm::EDGetTokenT<double> spinnerWeightFlipToken_;
      edm::EDGetTokenT<double> spinnerWeightMinusToken_;
      edm::EDGetTokenT<double> spinnerWeightPlusToken_;
      edm::EDGetTokenT<double> muEffWeightToken_;
      edm::EDGetTokenT<double> muEffWeightUpToken_;
      edm::EDGetTokenT<double> muEffWeightDownToken_;
      edm::EDGetTokenT<double> muRadWeightToken_;
      edm::EDGetTokenT<double> muRadWeightUpToken_;
      edm::EDGetTokenT<double> muRadWeightDownToken_;
      edm::EDGetTokenT<double> genTau2PtVsGenTau1PtGenToken_;
      edm::EDGetTokenT<double> genTau2EtaVsGenTau1EtaGenToken_;
      edm::EDGetTokenT<double> diTauMassVsGenDiTauPtGenToken_;
      edm::EDGetTokenT<double> genTau2PtVsGenTau1PtRecToken_;
      edm::EDGetTokenT<double> genTau2EtaVsGenTau1EtaRecToken_;
      edm::EDGetTokenT<double> diTauMassVsGenDiTauPtRecToken_; 
      bool                     genInfo_;
      bool                     recHit_;
      bool                     useMuRad_;
      std::string              mitName_;       //mitname
      EmbedWeightArr           *embedWeight_;          //event info
  };
}
#endif
