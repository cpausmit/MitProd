// $Id: FillerEvtSelData.cc,v 1.6 2010/03/18 20:21:00 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerEvtSelData.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EvtSelData.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "DataFormats/METReco/interface/BeamHaloSummary.h"
#include <bitset>

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerEvtSelData::FillerEvtSelData(const ParameterSet &cfg, const char *name,  bool active) : 
  BaseFiller(cfg,"EvtSelData",active),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkEvtSelDataBrn)),
  HBHENoiseFilterName_           (Conf().getUntrackedParameter<edm::InputTag>("HBHENoiseFilterName",
                                  edm::InputTag("HBHENoiseFilterResultProducer"))),  
  ECALDeadCellFilterName_        (Conf().getUntrackedParameter<edm::InputTag>("ECALDeadCellFilterName",
                                  edm::InputTag("EcalDeadCellTriggerPrimitiveFilter"))),  
  trackingFailureFilterName_     (Conf().getUntrackedParameter<edm::InputTag>("trackingFailureFilterName",
                                  edm::InputTag("trackingFailureFilter"))),  
  EEBadScFilterName_             (Conf().getUntrackedParameter<edm::InputTag>("EEBadScFilterName",
                                  edm::InputTag("eeBadScFilter"))),  
  ECALaserCorrFilterName_        (Conf().getUntrackedParameter<edm::InputTag>("ECALaserCorrFilterName",
                                  edm::InputTag("ecalLaserCorrFilter"))),  
  tkManyStripClusName_           (Conf().getUntrackedParameter<edm::InputTag>("tkManyStripClusName",
                                  edm::InputTag("manystripclus53X"))),  
  tkTooManyStripClusName_         (Conf().getUntrackedParameter<edm::InputTag>("tkTooManyStripClusName",
                                  edm::InputTag("toomanystripclus53X"))),  
  tkLogErrorTooManyClustersName_ (Conf().getUntrackedParameter<edm::InputTag>("tkLogErrorTooManyClustersName",
                                  edm::InputTag("logErrorTooManyClusters"))),  
  BeamHaloSummaryName_           (Conf().getUntrackedParameter<edm::InputTag>("BeamHaloSummaryName",
                                  edm::InputTag("BeamHaloSummary"))),  
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
  event.getByLabel(HBHENoiseFilterName_, HBHENoiseFilterHandle);
  bool isHBHENoiseFilter = *HBHENoiseFilterHandle;
  
  edm::Handle<bool> ECALDeadCellFilterHandle;
  event.getByLabel(ECALDeadCellFilterName_, ECALDeadCellFilterHandle);
  bool isECALDeadCellFilter = *ECALDeadCellFilterHandle;
  
  edm::Handle<bool> trackingFailureFilterHandle;
  event.getByLabel(trackingFailureFilterName_, trackingFailureFilterHandle);
  bool isTrackingFailureFilter = *trackingFailureFilterHandle;

  edm::Handle<bool> EEBadScFilterHandle;
  event.getByLabel(EEBadScFilterName_, EEBadScFilterHandle);
  bool isEEBadScFilter = *EEBadScFilterHandle;

  edm::Handle<bool> ECALaserCorrFilterHandle;
  event.getByLabel(ECALaserCorrFilterName_, ECALaserCorrFilterHandle);
  bool isECALaserCorrFilter = *ECALaserCorrFilterHandle;

  edm::Handle<bool> tkManyStripClusHandle;
  event.getByLabel(tkManyStripClusName_, tkManyStripClusHandle);
  edm::Handle<bool> tkTooManyStripClusHandle;
  event.getByLabel(tkTooManyStripClusName_, tkTooManyStripClusHandle);
  edm::Handle<bool> tkLogErrorTooManyClustersHandle;
  event.getByLabel(tkLogErrorTooManyClustersName_, tkLogErrorTooManyClustersHandle);
  //Odd tracking filter :three filters (with inverted logic)
  bool isTkOddManyStripClusFilter = !(*tkManyStripClusHandle);
  bool isTkOddTooManyStripClusFilter = !(*tkTooManyStripClusHandle);
  bool isTkOddLogErrorTooManyClustersFilter = !(*tkLogErrorTooManyClustersHandle);

  //Read the beam halo summary
  edm::Handle<reco::BeamHaloSummary> BeamHaloSummaryHandle;
  event.getByLabel(BeamHaloSummaryName_ , BeamHaloSummaryHandle);
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
