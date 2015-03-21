// $Id: FillerEvtSelData.cc,v 1.6 2010/03/18 20:21:00 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerEvtSelData.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EvtSelData.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include <bitset>

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerEvtSelData::FillerEvtSelData(const ParameterSet &cfg, edm::ConsumesCollector& collector, const char *name,  bool active) : 
  BaseFiller(cfg,"EvtSelData",active),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkEvtSelDataBrn)),
  HBHENoiseFilterToken_(GetToken<bool>(collector, "HBHENoiseFilterName", "HBHENoiseFilterResultProducer")),
  ECALDeadCellFilterToken_(GetToken<bool>(collector, "ECALDeadCellFilterName", "EcalDeadCellTriggerPrimitiveFilter")),
  trackingFailureFilterToken_(GetToken<bool>(collector, "trackingFailureFilterName", "trackingFailureFilter")),
  EEBadScFilterToken_(GetToken<bool>(collector, "EEBadScFilterName", "eeBadScFilter")),
  ECALaserCorrFilterToken_(GetToken<bool>(collector, "ECALaserCorrFilterName", "ecalLaserCorrFilter")),
  tkManyStripClusToken_(GetToken<bool>(collector, "tkManyStripClusName", "manystripclus53X")),
  tkTooManyStripClusToken_(GetToken<bool>(collector, "tkTooManyStripClusName", "toomanystripclus53X")),
  tkLogErrorTooManyClustersToken_(GetToken<bool>(collector, "tkLogErrorTooManyClustersName", "logErrorTooManyClusters")),
  BeamHaloSummaryToken_(GetToken<reco::BeamHaloSummary>(collector, "BeamHaloSummaryName", "BeamHaloSummary")),
  evtSelData_(new EvtSelData())
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerEvtSelData::~FillerEvtSelData()
{
  // Destructor.

  delete evtSelData_;
}

//--------------------------------------------------------------------------------------------------
void FillerEvtSelData::BookDataBlock(TreeWriter &tws)
{
  // Book our branch.

  tws.AddBranch(mitName_,&evtSelData_);
  OS()->add<mithep::EvtSelData>(evtSelData_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerEvtSelData::FillDataBlock(const edm::Event &event, 
                                     const edm::EventSetup &setup)
{
  //Read the MET filters boolean decisions
  edm::Handle<bool> HBHENoiseFilterHandle;
  GetProduct(HBHENoiseFilterToken_, HBHENoiseFilterHandle, event);
  bool isHBHENoiseFilter = *HBHENoiseFilterHandle;
  
  edm::Handle<bool> ECALDeadCellFilterHandle;
  GetProduct(ECALDeadCellFilterToken_, ECALDeadCellFilterHandle, event);
  bool isECALDeadCellFilter = *ECALDeadCellFilterHandle;
  
  edm::Handle<bool> trackingFailureFilterHandle;
  GetProduct(trackingFailureFilterToken_, trackingFailureFilterHandle, event);
  bool isTrackingFailureFilter = *trackingFailureFilterHandle;

  edm::Handle<bool> EEBadScFilterHandle;
  GetProduct(EEBadScFilterToken_, EEBadScFilterHandle, event);
  bool isEEBadScFilter = *EEBadScFilterHandle;

  edm::Handle<bool> ECALaserCorrFilterHandle;
  GetProduct(ECALaserCorrFilterToken_, ECALaserCorrFilterHandle, event);
  bool isECALaserCorrFilter = *ECALaserCorrFilterHandle;

  edm::Handle<bool> tkManyStripClusHandle;
  GetProduct(tkManyStripClusToken_, tkManyStripClusHandle, event);
  edm::Handle<bool> tkTooManyStripClusHandle;
  GetProduct(tkTooManyStripClusToken_, tkTooManyStripClusHandle, event);
  edm::Handle<bool> tkLogErrorTooManyClustersHandle;
  GetProduct(tkLogErrorTooManyClustersToken_, tkLogErrorTooManyClustersHandle, event);
  //Odd tracking filter :three filters (with inverted logic)
  bool isTkOddManyStripClusFilter = !(*tkManyStripClusHandle);
  bool isTkOddTooManyStripClusFilter = !(*tkTooManyStripClusHandle);
  bool isTkOddLogErrorTooManyClustersFilter = !(*tkLogErrorTooManyClustersHandle);

  //Read the beam halo summary
  edm::Handle<reco::BeamHaloSummary> BeamHaloSummaryHandle;
  GetProduct(BeamHaloSummaryToken_ , BeamHaloSummaryHandle, event);
  bool isCSCTightHaloFilter = !BeamHaloSummaryHandle->CSCTightHaloId();
  bool isCSCLooseHaloFilter = !BeamHaloSummaryHandle->CSCLooseHaloId();

  //Crwate the bit word
  int thisEventWord = GetMetFiltersWord (
                      isHBHENoiseFilter, isECALDeadCellFilter,
                      isTrackingFailureFilter, isEEBadScFilter,
                      isECALaserCorrFilter, isTkOddManyStripClusFilter,
                      isTkOddTooManyStripClusFilter, isTkOddLogErrorTooManyClustersFilter,
                      isCSCTightHaloFilter, isCSCLooseHaloFilter);
  
  //Store the bit word in the container
  evtSelData_->SetFiltersWord(thisEventWord);
}

//--------------------------------------------------------------------------------------------------
int FillerEvtSelData::GetMetFiltersWord(
                      Bool_t HBHENoiseFilter, Bool_t ECALDeadCellFilter,
                      Bool_t trackingFailureFilter, Bool_t EEBadScFilter,
                      Bool_t ECALaserCorrFilter, Bool_t tkOddManyStripClusFilter,
                      Bool_t tkOddTooManyStripClusFilter, Bool_t tkOddLogErrorTooManyClustersFilter,
                      Bool_t CSCTightHaloFilter, Bool_t CSCLooseHaloFilter)
				    
{
  // This function creates the word containing the bit decisions.
  // The bit ordering follows the order of the parameters passed  
  // to this function. For more information about the bit meaning go to
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFilters

  //Initialize the word
  int theWord = 0;
  //Initialize the vector of bits
  std::vector<int> theBits;
  theBits.push_back((int) HBHENoiseFilter);
  theBits.push_back((int) ECALDeadCellFilter);
  theBits.push_back((int) trackingFailureFilter);
  theBits.push_back((int) EEBadScFilter);
  theBits.push_back((int) ECALaserCorrFilter);
  theBits.push_back((int) tkOddManyStripClusFilter);
  theBits.push_back((int) tkOddTooManyStripClusFilter);
  theBits.push_back((int) tkOddLogErrorTooManyClustersFilter);
  theBits.push_back((int) CSCTightHaloFilter);
  theBits.push_back((int) CSCLooseHaloFilter);
  //Create the word
  for (unsigned int iBit = 0; iBit < theBits.size(); iBit++)
    theWord |= theBits[iBit] << iBit;
  
  return theWord;
}
