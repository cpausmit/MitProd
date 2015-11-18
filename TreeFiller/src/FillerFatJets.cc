#include "MitProd/TreeFiller/interface/FillerFatJets.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/GeometrySurface/interface/Line.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"

#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputerWrapper.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"

unsigned const N_MAX_SUBJETS = 4;

mithep::FillerFatJets::FillerFatJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  FillerPFJets(cfg, collector, os, name, active),
  fR0(cfg.getUntrackedParameter<double>("R0", 0.8)),
  fSubjetNames(cfg.getUntrackedParameter<std::vector<std::string> >("SubJetLabels")),
  fPVToken(GetToken<reco::VertexCollection>(collector,cfg,"edmPrimaryVertices")),
  njettiness(fastjet::contrib::OnePass_KT_Axes(), fastjet::contrib::NormalizedMeasure(1.0,fR0))
{
  fillFromPAT_ = true;
  jets_ = new mithep::FatJetArr(4);

  auto subjetTags(cfg.getUntrackedParameter<std::vector<std::string> >("SubJets"));
  for (auto& tag : subjetTags)
    fSubjetCollectionTokens.push_back(collector.consumes<PatJetCollection>(edm::InputTag(tag)));
}

mithep::FillerFatJets::~FillerFatJets()
{
}

void
mithep::FillerFatJets::BookAdditional(TreeWriter &tws)
{
}

void
mithep::FillerFatJets::PrepareSpecific(edm::Event const& iEvent, edm::EventSetup const&)
{
  unsigned int nSubjetTypes = fSubjetCollectionTokens.size();
  fSubjetCollections.resize(nSubjetTypes);
  for (size_t i = 0; i < nSubjetTypes; ++i)
    GetProduct(fSubjetCollectionTokens[i], fSubjetCollections[i], iEvent);
  GetProduct(fPVToken,fPVs,iEvent);
}

void
mithep::FillerFatJets::FillSpecific(mithep::Jet& outBaseJet, reco::JetBaseRef const& inJetRef)
{
  auto& outJet = static_cast<mithep::FatJet&>(outBaseJet);
  auto* inJet = dynamic_cast<pat::Jet const*>(inJetRef.get());
  if (!inJet)
    throw edm::Exception(edm::errors::Configuration, "FillerFatJets::FillSpecific)")
      << "fillFromPAT set on non-PAT input";

  // pat::Jet::p4() returns a corrected p4. Uncorrect here
  double toRaw = inJet->jecFactor("Uncorrected");
  auto&& rawP4 = inJet->p4() * toRaw;
  outJet.SetRawPtEtaPhiM(rawP4.pt(), rawP4.eta(), rawP4.phi(), rawP4.mass());

  fillPATFatJetVariables(outJet, *inJet);
}

void
mithep::FillerFatJets::fillPATFatJetVariables(mithep::FatJet& outJet, pat::Jet const& inJet)
{
  fillPATJetVariables(outJet,inJet);
  outJet.SetCharge();
  std::map<float,float> btagMap; // maps pT to btag
  // now let's save subjet btag
  for (auto & subjetName : fSubjetNames) { // loop over subjet types
    const PatJetPtrCollection & subjets = inJet.subjets(subjetName);
    for (auto & inSubjetPtr : subjets) {
      pat::Jet const& inSubjet(*inSubjetPtr);
      btagMap[inSubjet.p4().pt()] = inSubjet.bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
    }
    for(std::map<float,float>::reverse_iterator iBtag=btagMap.rbegin(); iBtag!=btagMap.rend(); ++iBtag)
      outJet.AddSubJetBtag(iBtag->second);
  }

  // now let's tag some bs
  // first grab pointers to info
  const IPTagInfo* ipTagInfo = inJet.tagInfoCandIP("pfImpactParameter");
  const SVTagInfo* svTagInfo = inJet.tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder");
  const reco::CandSoftLeptonTagInfo* softPFMuTagInfo = inJet.tagInfoCandSoftLepton("softPFMuonsTagInfos");
  const reco::CandSoftLeptonTagInfo* softPFElTagInfo = inJet.tagInfoCandSoftLepton("softPFElectronsTagInfos");
  const reco::Vertex* pv = &(*fPVs->begin());

  // recalculate nsubjettiness after grouping svx tracks into one subjet
  std::vector<fastjet::PseudoJet> currentAxes;
  recalcNsubjettiness(inJet,*svTagInfo,outJet,currentAxes);

  // now let's store tracking info
  int nSelTracks=0;
  const Tracks & selectedTracks( ipTagInfo->selectedTracks() );
  reco::TrackKinematics allKinematics;
  unsigned int trackSize = selectedTracks.size();
  for (unsigned int itt=0; itt < trackSize; ++itt) {
    TrackData trackData;
    const reco::Track & ptrack = *(reco::btag::toTrack(selectedTracks[itt]));
    const TrackRef ptrackRef = selectedTracks[itt];

    trackData.length = (ipTagInfo->impactParameterData()[itt].closestToJetAxis - RecoVertex::convertPos(pv->position())).mag();
    trackData.dist = ipTagInfo->impactParameterData()[itt].distanceToJetAxis.value();
    trackData.dxy = ptrack.dxy(pv->position());
    trackData.dz = ptrack.dz(pv->position());

    double deltaR = reco::deltaR( ptrack.eta(), ptrack.phi(), inJet.p4().eta(), inJet.p4().phi());
    if(deltaR<0.3) nSelTracks++;
    // first impact parameter
    trackData.IP2D     = ipTagInfo->impactParameterData()[itt].ip2d.value();
    trackData.IP2Dsig  = ipTagInfo->impactParameterData()[itt].ip2d.significance();
    trackData.IP       = ipTagInfo->impactParameterData()[itt].ip3d.value();
    trackData.IPsig    = ipTagInfo->impactParameterData()[itt].ip3d.significance();
    trackData.IP2Derr  = ipTagInfo->impactParameterData()[itt].ip2d.error();
    trackData.IPerr    = ipTagInfo->impactParameterData()[itt].ip3d.error();
    trackData.prob     = ipTagInfo->probabilities(0)[itt];
    // then track kinematics and hits
    trackData.pt       = ptrack.pt();
    trackData.eta      = ptrack.eta();
    trackData.phi      = ptrack.phi();

    setTracksPV(ptrackRef, fPVs, trackData.PV, trackData.PVWeight);
    if (!trackData.PV && trackData.PVWeight > 0)
      allKinematics.add(ptrack,trackData.PVWeight);
    if (inJet.hasTagInfo("pfInclusiveSecondaryVertexFinder")) {
      setTracksSV(ptrackRef,svTagInfo,trackData.fromSV, trackData.SV, trackData.SVWeight);
    } else {
      trackData.fromSV = 0;
      trackData.SV = -1;
      trackData.SVWeight = 0;
    }
    outJet.AddTrackData(&trackData);
  }
  math::XYZTLorentzVector allSum =  allKinematics.weightedVectorSum() ; //allKinematics.vectorSum()

  // organize secondary vertices by mass
  std::map<double, unsigned int> VTXmass;
  unsigned int nSelectedSV = 0;
  double maxSVDeltaR2ToJet = fR0-0.1+(fR0-0.8)*0.1/0.7;  //linear interpolation
  maxSVDeltaR2ToJet = maxSVDeltaR2ToJet*maxSVDeltaR2ToJet;
  edm::RefToBase<reco::Jet> rJet = ipTagInfo->jet();
  math::XYZVector jetDir = rJet->momentum().Unit();
  for (unsigned int vtx = 0; vtx < svTagInfo->nVertices(); ++vtx)  {
    const recoVertexPtr &vertex = svTagInfo->secondaryVertex(vtx);
    double mass = vertex.p4().mass();
    GlobalVector flightDir = svTagInfo->flightDirection(vtx);
    if (reco::deltaR2(flightDir, jetDir)<maxSVDeltaR2ToJet) {
      // only keep nearby svxs
      VTXmass[mass]=vtx;
      ++nSelectedSV;
    }
  }

  GlobalVector flightDir0, flightDir1;
  int cont=0;
  reco::Candidate::LorentzVector svP4_0 , svP4_1;
  // fill FatJet::fSVData in order of decreasing secondary vertex mass
  for ( std::map<double, unsigned int>::reverse_iterator iVtx=VTXmass.rbegin(); iVtx!=VTXmass.rend(); ++iVtx) {
    ++cont;
    SVData svData;
    unsigned int idx = iVtx->second;
    const recoVertexPtr &vertex = svTagInfo->secondaryVertex(idx);
    //svx kinematics
    svData.mass = iVtx->first;
    svData.pt = vertex.p4().pt();
    svData.eta = vertex.p4().eta();
    svData.phi = vertex.p4().phi();
    //find svx energy fraction and charge
    Int_t totcharge=0;
    reco::TrackKinematics vertexKinematics;
    vertexKinematicsAndCharge(vertex, vertexKinematics, totcharge);
    math::XYZTLorentzVector vertexSum = vertexKinematics.weightedVectorSum();
    svData.energyRatio = vertexSum.E()/allSum.E();
    svData.totCharge = totcharge;

    GlobalVector flightDir = svTagInfo->flightDirection(idx);
    svData.dirX = flightDir.x();
    svData.dirY = flightDir.y();
    svData.dirZ = flightDir.z();
    svData.deltaRJet = reco::deltaR(flightDir, jetDir);
    svData.deltaRSumJet = reco::deltaR(vertexSum, jetDir);
    svData.deltaRSumDir = reco::deltaR(vertexSum, flightDir);

    Line::PositionType pos(GlobalPoint(vertex.position().x(),vertex.position().y(),vertex.position().z()));
    Line trackline(pos,flightDir);
    Line::PositionType pos2(GlobalPoint(pv->x(),pv->y(),pv->z()));
    Line::DirectionType dir2(GlobalVector(jetDir.x(),jetDir.y(),jetDir.z()));
    Line jetline(pos2,dir2);
    svData.vtxDistJetAxis = (jetline.distance(trackline)).mag();

    svData.flight = svTagInfo->flightDistance(idx).value();
    svData.flightErr = svTagInfo->flightDistance(idx).error();
    svData.flight2D = svTagInfo->flightDistance(idx, true).value();
    svData.flight2DErr = svTagInfo->flightDistance(idx, true).error();
    svData.nTrk = svTagInfo->secondaryVertex(idx).numberOfSourceCandidatePtrs();
    outJet.AddSVData(&svData);

    if (cont==1) {
      flightDir0 = svTagInfo->flightDirection(iVtx->second);
      svP4_0= vertex.p4();
      double tauDot_tmp;
      if (reco::deltaR2(flightDir0,currentAxes[1])<reco::deltaR2(flightDir0,currentAxes[0]))
        tauDot_tmp = (currentAxes[1].px()*flightDir0.x()+currentAxes[1].py()*flightDir0.y()+currentAxes[1].pz()*flightDir0.z())/(sqrt(currentAxes[1].modp2())*flightDir0.mag());
      else
        tauDot_tmp = (currentAxes[0].px()*flightDir0.x()+currentAxes[0].py()*flightDir0.y()+currentAxes[0].pz()*flightDir0.z())/(sqrt(currentAxes[0].modp2())*flightDir0.mag());
      outJet.SetTauDot(tauDot_tmp);
    }
    if (cont==2) {
      flightDir1 = svTagInfo->flightDirection(iVtx->second);
      svP4_1 = vertex.p4();
      outJet.SetZRatio(reco::deltaR(flightDir0,flightDir1)*(svP4_0.pt())/(svP4_0+svP4_1).mass());
    }
  }
  // muons!
  if (softPFMuTagInfo) {
    int nSM = softPFMuTagInfo->leptons();

    for (size_t leptIdx = 0; leptIdx < (size_t)nSM; ++leptIdx) {
      LeptonData muonData;
      muonData.pt        = softPFMuTagInfo->lepton(leptIdx)->pt();
      muonData.eta       = softPFMuTagInfo->lepton(leptIdx)->eta();
      muonData.phi       = softPFMuTagInfo->lepton(leptIdx)->phi();
      muonData.ptRel     = (softPFMuTagInfo->properties(leptIdx).ptRel);
      muonData.ratio     = (softPFMuTagInfo->properties(leptIdx).ratio);
      muonData.ratioRel  = (softPFMuTagInfo->properties(leptIdx).ratioRel);
      muonData.IP        = (softPFMuTagInfo->properties(leptIdx).sip3d);
      muonData.IP2D      = (softPFMuTagInfo->properties(leptIdx).sip2d);
      outJet.AddMuonData(&muonData);
    }
  }
  // electrons!
  if (softPFElTagInfo) {
    int nSE = softPFElTagInfo->leptons();
    for (size_t leptIdx = 0; leptIdx < (size_t)nSE; ++leptIdx) {
      LeptonData electronData;
      electronData.pt        = softPFElTagInfo->lepton(leptIdx)->pt();
      electronData.eta       = softPFElTagInfo->lepton(leptIdx)->eta();
      electronData.phi       = softPFElTagInfo->lepton(leptIdx)->phi();
      electronData.ptRel     = (softPFElTagInfo->properties(leptIdx).ptRel);
      electronData.ratio     = (softPFElTagInfo->properties(leptIdx).ratio);
      electronData.ratioRel  = (softPFElTagInfo->properties(leptIdx).ratioRel);
      electronData.IP        = (softPFElTagInfo->properties(leptIdx).sip3d);
      electronData.IP2D      = (softPFElTagInfo->properties(leptIdx).sip2d);
      outJet.AddElectronData(&electronData);
    }
  }

  // set btags for fatjet
  setBTagDiscriminators(outJet, inJet);

}

void mithep::FillerFatJets::setBTagDiscriminators(mithep::Jet & outJet, pat::Jet const & inJet)
{
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfJetProbabilityBJetTags"), Jet::kJetProbability);
/*
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfNegativeOnlyJetProbabilityBJetTags"), Jet::kJetProbabilityNegative);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfPositiveOnlyJetProbabilityBJetTags"), Jet::kJetProbabilityPositive);
*/

  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfJetProbabilityBJetTags"), Jet::kJetBProbability);
/*
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfNegativeOnlyJetBProbabilityBJetTags"), Jet::kJetBProbabilityNegative);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfPositiveOnlyJetBProbabilityBJetTags"), Jet::kJetBProbabilityPositive);
*/

/*
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfSimpleSecondaryVertexHighEffBJetTags"), Jet::kSimpleSecondaryVertexHighEff);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfNegativeSimpleSecondaryVertexHighEffBJetTags"), Jet::kSimpleSecondaryVertexHighEffNegative);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfSimpleSecondaryVertexHighPurBJetTags"), Jet::kSimpleSecondaryVertexHighPur);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfNegativeSimpleSecondaryVertexHighPurBJetTags"), Jet::kSimpleSecondaryVertexHighPurNegative);
*/

  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfCombinedSecondaryVertexV2BJetTags"), Jet::kCombinedSecondaryVertexV2);
/*
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfPositiveCombinedSecondaryVertexV2BJetTags"), Jet::kCombinedSecondaryVertexV2Positive);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfNegativeCombinedSecondaryVertexV2BJetTags"), Jet::kCombinedSecondaryVertexV2Negative);
*/

  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"), Jet::kCombinedInclusiveSecondaryVertexV2);
/*
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfPositiveCombinedInclusiveSecondaryVertexV2BJetTags"), Jet::kCombinedInclusiveSecondaryVertexV2Positive);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfNegativeCombinedInclusiveSecondaryVertexV2BJetTags"), Jet::kCombinedInclusiveSecondaryVertexV2Negative);
*/

/*
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("softPFMuonBJetTags"), Jet::kSoftPFMuon);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("negativeSoftPFMuonBJetTags"), Jet::kSoftPFMuonNegative);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("positiveSoftPFMuonBJetTags"), Jet::kSoftPFMuonPositive);

  outJet.SetBJetTagsDisc(inJet.bDiscriminator("softPFElectronBJetTags"), Jet::kSoftPFElectron);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("negativeSoftPFElectronBJetTags"), Jet::kSoftPFElectronNegative);
  outJet.SetBJetTagsDisc(inJet.bDiscriminator("positiveSoftPFElectronBJetTags"), Jet::kSoftPFElectronPositive);
*/

  outJet.SetBJetTagsDisc(inJet.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags"), Jet::kDoubleSecondaryVertex);
}

void
mithep::FillerFatJets::vertexKinematicsAndCharge(const recoVertexPtr & vertex, reco::TrackKinematics & vertexKinematics, Int_t & charge)
{
  const std::vector<reco::CandidatePtr> & tracks = vertex.daughterPtrVector();

  for(auto&& track = tracks.begin(); track != tracks.end(); ++track) {
    const reco::Track& mytrack = *(*track)->bestTrack();
    vertexKinematics.add(mytrack, 1.0);
    charge+=mytrack.charge();
  }
}

void
mithep::FillerFatJets::setTracksSV(const TrackRef & trackRef, const SVTagInfo * svTagInfo, int & isFromSV, int & iSV, float & SVweight)
{
  isFromSV = 0;
  iSV = -1;
  SVweight = 0.;

  size_t nSV = svTagInfo->nVertices();
  for(size_t iv=0; iv<nSV; ++iv)  {
    const recoVertexPtr & vtx = svTagInfo->secondaryVertex(iv);
    // one of the tracks in the vertex is the same as the track considered in the function
    const std::vector<reco::CandidatePtr> & tracks = vtx.daughterPtrVector();
    if(std::find(tracks.begin(),tracks.end(),trackRef) != tracks.end()) {
      SVweight = 1.;
      isFromSV = 1;
      iSV = iv;
      // select the first vertex for which the track is used in the fit
    }
    // (reco::VertexCompositePtrCandidate does not store track weights so can't select the vertex for which the track has the highest weight)
    if(iSV>=0)
      break;
  }
}

void mithep::FillerFatJets::setTracksPV(const TrackRef & trackRef, const edm::Handle<reco::VertexCollection> & pvHandle, int & iPV, float & PVweight)
{
  iPV = -1;
  PVweight = 0.;
  reco::PFCandidate const* pfcand = static_cast<reco::PFCandidate const*>(trackRef.get());
  setTracksPVBase(pfcand->trackRef(), pvHandle, iPV, PVweight);
}

void mithep::FillerFatJets::setTracksPVBase(const reco::TrackRef & trackRef, const edm::Handle<reco::VertexCollection> & pvHandle, int & iPV, float & PVweight)
{
  iPV = -1;
  PVweight = 0.;
  const reco::TrackBaseRef trackBaseRef(trackRef);

  for(auto&& iv = pvHandle->begin(); iv != pvHandle->end(); ++iv) {
    const reco::Vertex& vtx = *iv;
    // loop over tracks in vertices
    for(auto&& it = vtx.tracks_begin(); it != vtx.tracks_end(); ++it) {
      const reco::TrackBaseRef & baseRef = *it;
      // one of the tracks in the vertex is the same as the track considered in the function
      if(baseRef == trackBaseRef) {
        double w = vtx.trackWeight(baseRef);
        // select the vertex for which the track has the highest weight
        if(w > PVweight) {
          PVweight = w;
          iPV = (iv - pvHandle->begin());
          break;
        }
      }
    }
  }
}

void
mithep::FillerFatJets::recalcNsubjettiness(const pat::Jet &jet,
                                           const SVTagInfo &svTagInfo,
                                           mithep::FatJet &outJet,
                                           std::vector<fastjet::PseudoJet> &currentAxes)
{
  // recalculate nsubjettiness removing the bjet
  std::vector<fastjet::PseudoJet> fjParticles;
  std::vector<reco::CandidatePtr> svDaughters;

  // loop over IVF vertices and push them in the vector of FastJet constituents and also collect their daughters
  for(size_t i=0; i<svTagInfo.nVertices(); ++i) {
    const reco::VertexCompositePtrCandidate & vtx = svTagInfo.secondaryVertex(i);

    fjParticles.push_back(fastjet::PseudoJet( vtx.px(), vtx.py(), vtx.pz(), vtx.energy()));

    const std::vector<reco::CandidatePtr> & daughters = vtx.daughterPtrVector();
    svDaughters.insert(svDaughters.end(), daughters.begin(), daughters.end());
  }

  // loop over jet constituents and select those that are not daughters of IVF vertices
  std::vector<reco::CandidatePtr> constituentsOther;
  for(const reco::CandidatePtr & daughter : jet.daughterPtrVector()) {
    if (std::find(svDaughters.begin(), svDaughters.end(), daughter) == svDaughters.end())
      constituentsOther.push_back( daughter );
  }

  // loop over jet constituents that are not daughters of IVF vertices and push them in the vector of FastJet constituents
  for(const reco::CandidatePtr & constit : constituentsOther) {
    if (constit.isNonnull() && constit.isAvailable())
      fjParticles.push_back( fastjet::PseudoJet( constit->px(), constit->py(), constit->pz(), constit->energy() ) );
    else
      edm::LogWarning("MissingJetConstituent") << "Jet constituent required for N-subjettiness computation is missing!";
  }

  // re-calculate N-subjettiness
  outJet.SetTau1IVF(njettiness.getTau(1, fjParticles));
  outJet.SetTau2IVF(njettiness.getTau(2, fjParticles));
  currentAxes = njettiness.currentAxes();
  outJet.SetTau3IVF(njettiness.getTau(3, fjParticles));
  for (auto & axis : njettiness.currentAxes()) {
    outJet.AddTauIVFAxis(Vect3(axis.px(),axis.py(),axis.pz()));
  }
}

DEFINE_MITHEP_TREEFILLER(FillerFatJets);
