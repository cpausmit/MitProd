//--------------------------------------------------------------------------------------------------
// FillerFatJets
//
// Implementation of a filler to fill EDM particle flow jets into our mithep::PFJet data structure.
//
// Authors: C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERFATJETS_H
#define MITPROD_TREEFILLER_FILLERFATJETS_H

#include "MitProd/TreeFiller/interface/FillerJets.h"
#include "MitProd/TreeFiller/interface/FillerPFJets.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/FatJetCol.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/BTauReco/interface/CandIPTagInfo.h"
#include "DataFormats/BTauReco/interface/TrackIPTagInfo.h"
#include "DataFormats/BTauReco/interface/CandSoftLeptonTagInfo.h"
#include "RecoBTag/SecondaryVertex/interface/TemplatedSimpleSecondaryVertexComputer.h"
#include "RecoBTag/SecondaryVertex/interface/TrackKinematics.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/contrib/Njettiness.hh"



namespace pat {
  class Jet;
}

typedef std::vector<pat::Jet> PatJetCollection;
typedef std::vector<edm::Ptr<pat::Jet> > PatJetPtrCollection;

namespace mithep {

  class FillerFatJets : public FillerPFJets {
  public:
    typedef mithep::FatJet::TrackData TrackData;
    typedef mithep::FatJet::SVData SVData;
    typedef reco::CandIPTagInfo IPTagInfo;
    typedef reco::VertexCompositePtrCandidate recoVertex;
    typedef reco::TemplatedSecondaryVertexTagInfo<IPTagInfo,recoVertex> SVTagInfo;
    typedef typename IPTagInfo::input_container Tracks;
    typedef typename IPTagInfo::input_container::value_type TrackRef;

    FillerFatJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerFatJets();

    mithep::Jet* AddNew() override { return static_cast<mithep::FatJetArr*>(jets_)->AddNew(); }
    void FillSpecific(mithep::Jet&, reco::JetBaseRef const&);
    void FillSpecificSubjet(mithep::XlSubJet&, edm::Ptr<pat::Jet>);
    void PrepareSpecific(edm::Event const&, edm::EventSetup const&) override;
    reco::JetTagCollection const* fBJetTags;
  private:
    void fillPATFatJetVariables(mithep::FatJet&, pat::Jet const&);
    void recalcNsubjettiness(const pat::Jet &, const SVTagInfo &, mithep::FatJet &, std::vector<fastjet::PseudoJet> &);
    void vertexKinematicsAndCharge(const recoVertex & vertex, reco::TrackKinematics & vertexKinematics, Int_t & charge);
    void setTracksPV(const TrackRef & trackRef, const edm::Handle<reco::VertexCollection> & pvHandle, int & iPV, double & PVweight);
    void setTracksPVBase(const reco::TrackRef & trackRef, const edm::Handle<reco::VertexCollection> & pvHandle, int & iPV, double & PVweight);
    void setTracksSV (const TrackRef & trackRef, const SVTagInfo * svTagInfo, int & isFromSV, int & iSV, double & SVweight);

    double fR0;                                                       //cone size
    std::vector<std::string> fSubjetNames;                            //labels of subjets
    std::vector<edm::InputTag> fSubjetCollectionTags;                 //subjet input tags
    std::vector<edm::Handle<PatJetCollection> > fSubjetCollections;   //vector of vector of pat subjets
    edm::EDGetTokenT<reco::VertexCollection> fPVToken;                //offline primary vertex token
    edm::Handle<reco::VertexCollection> fPVs;                         //offline primary vertices
    fastjet::contrib::Njettiness njettiness;                          //used to recompute njettiness

  };
}

#endif
