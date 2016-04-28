#include "MitProd/TreeFiller/interface/FillerMCVertexes.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/BaseVertexCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/TreeFiller/interface/VertexMatch.h"

//--------------------------------------------------------------------------------------------------
mithep::FillerMCVertexes::FillerMCVertexes(const edm::ParameterSet &cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg, os, name, active),
  simActive_(cfg.getUntrackedParameter("simActive", true)),
  genSource_(nGenSources),
  hepMCProdToken_(),
  genParticlesToken_(),
  simVerticesToken_(),
  mitName_(cfg.getUntrackedParameter("mitName", std::string("MCVertexes"))),
  vertexes_(new mithep::BaseVertexArr(16, mitName_.c_str())),
  genVtxMap_(0),
  simVtxMap_(0)
{
  std::string genSourceName(cfg.getUntrackedParameter("genSource", std::string("GenParticleCollection")));
  if (genSourceName == "GenParticleCollection")
    genSource_ = kGenParticles;
  else if (genSourceName == "PackedGenParticleCollection")
    genSource_ = kPackedGenParticles;
  else if (genSourceName == "HepMCProduct")
    genSource_ = kHepMCProduct;
  else
    throw edm::Exception(edm::errors::Configuration, "FillerMCVertexes::Constructor")
      << "Parameter genSource must be either one of GenParticleCollection, PackedGenParticleCollection,"
      << " or HepMCProduct.";

  switch (genSource_) {
  case kGenParticles:
    genParticlesToken_ = GetToken<reco::GenParticleCollection>(collector, cfg, "edmName");
    break;
  case kPackedGenParticles:
    packedGenParticlesToken_ = GetToken<pat::PackedGenParticleCollection>(collector, cfg, "edmName");
    break;
  case kHepMCProduct:
    GetToken<edm::HepMCProduct>(collector, cfg, "edmName");
    genVtxMapName_ = cfg.getUntrackedParameter("vtxMapName", std::string(""));
    genVtxMap_ = new mithep::GenVertexMap;
    break;
  default:
    break;
  }

  if (simActive_) {
    simVerticesToken_ = GetToken<edm::SimVertexContainer>(collector, cfg, "simEdmName");
    simVtxMapName_ = cfg.getUntrackedParameter<std::string>("simVtxMapName", "");
    simVtxMap_ = new mithep::SimVertexMap;
  }
}

//--------------------------------------------------------------------------------------------------
mithep::FillerMCVertexes::~FillerMCVertexes()
{
  delete vertexes_;
  delete genVtxMap_;
  delete simVtxMap_;
}

//--------------------------------------------------------------------------------------------------
void mithep::FillerMCVertexes::BookDataBlock(TreeWriter &tws)
{
  // Add branch to tree and publish maps.

  tws.AddBranch(mitName_,&vertexes_);
  OS()->add<BaseVertexArr>(vertexes_,mitName_);

  if (genVtxMap_ && !genVtxMapName_.empty()) {
    genVtxMap_->SetBrName(mitName_);
    OS()->add(genVtxMap_, genVtxMapName_);
  }
  if (simVtxMap_ && !simVtxMapName_.empty()) {
    simVtxMap_->SetBrName(mitName_);
    OS()->add(simVtxMap_, simVtxMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void mithep::FillerMCVertexes::FillDataBlock(const edm::Event      &event, 
                                             const edm::EventSetup &setup)
{
  // Loop over generated or simulated particles and fill their information.

  vertexes_->Delete();

  // just for fun - set vertex position "error" based on RMS
  struct RMSCalc {
    unsigned nRefs{0};
    double sumdx{0.};
    double sumdy{0.};
    double sumdz{0.};
    double sumdx2{0.};
    double sumdy2{0.};
    double sumdz2{0.};
  };

  if (genSource_ == kGenParticles) {
    std::map<mithep::BaseVertex const*, RMSCalc> rmsCalc;

    edm::Handle<reco::GenParticleCollection> hGenPProduct;
    GetProduct(genParticlesToken_, hGenPProduct, event);  
    reco::GenParticleCollection const& genParticles = *hGenPProduct;

    for (auto&& inPart : genParticles) {
      mithep::ThreeVector offset;
      mithep::BaseVertex const* vtx = VertexMatch(vertexes_, inPart, &offset);

      if (!vtx) {
        mithep::BaseVertex* ptr = vertexes_->Allocate();
        new (ptr) mithep::BaseVertex(inPart.vx(), inPart.vy(), inPart.vz());
        vtx = ptr;
      }

      auto& calc(rmsCalc[vtx]);
      calc.nRefs += 1;
      calc.sumdx += offset.X();
      calc.sumdy += offset.Y();
      calc.sumdz += offset.Z();
      calc.sumdx2 += offset.X() * offset.X();
      calc.sumdy2 += offset.Y() * offset.Y();
      calc.sumdz2 += offset.Z() * offset.Z();
    }

    for (auto& vtxCalc : rmsCalc) {
      auto* vtx = const_cast<mithep::BaseVertex*>(vtxCalc.first);
      auto& calc(vtxCalc.second);
      vtx->SetErrors(std::sqrt(calc.sumdx2 / calc.nRefs - std::pow(calc.sumdx / calc.nRefs, 2.)),
                     std::sqrt(calc.sumdy2 / calc.nRefs - std::pow(calc.sumdy / calc.nRefs, 2.)),
                     std::sqrt(calc.sumdz2 / calc.nRefs - std::pow(calc.sumdz / calc.nRefs, 2.)));
    }
  }
  else if (genSource_ == kPackedGenParticles) {
    std::map<mithep::BaseVertex const*, RMSCalc> rmsCalc;

    edm::Handle<pat::PackedGenParticleCollection> hGenPProduct;
    GetProduct(packedGenParticlesToken_, hGenPProduct, event);  
    pat::PackedGenParticleCollection const& genParticles = *hGenPProduct;

    for (auto&& inPart : genParticles) {
      mithep::ThreeVector offset;
      mithep::BaseVertex const* vtx = VertexMatch(vertexes_, inPart, &offset);

      if (!vtx) {
        mithep::BaseVertex* ptr = vertexes_->Allocate();
        new (ptr) mithep::BaseVertex(inPart.vx(), inPart.vy(), inPart.vz());
        vtx = ptr;
      }

      auto& calc(rmsCalc[vtx]);
      calc.nRefs += 1;
      calc.sumdx += offset.X();
      calc.sumdy += offset.Y();
      calc.sumdz += offset.Z();
      calc.sumdx2 += offset.X() * offset.X();
      calc.sumdy2 += offset.Y() * offset.Y();
      calc.sumdz2 += offset.Z() * offset.Z();
    }

    for (auto& vtxCalc : rmsCalc) {
      auto* vtx = const_cast<mithep::BaseVertex*>(vtxCalc.first);
      auto& calc(vtxCalc.second);
      vtx->SetErrors(std::sqrt(calc.sumdx2 / calc.nRefs - std::pow(calc.sumdx / calc.nRefs, 2.)),
                     std::sqrt(calc.sumdy2 / calc.nRefs - std::pow(calc.sumdy / calc.nRefs, 2.)),
                     std::sqrt(calc.sumdz2 / calc.nRefs - std::pow(calc.sumdz / calc.nRefs, 2.)));
    }
  }
  else if (genSource_ == kHepMCProduct) {
    edm::Handle<edm::HepMCProduct> hHepMCProduct;
    GetProduct(hepMCProdToken_, hHepMCProduct, event);
    const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  

    for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
         pgen != GenEvent.particles_end(); ++pgen) {

      HepMC::GenParticle *mcPart = (*pgen);
      if(!mcPart) 
        continue;

      HepMC::GenVertex *pvtx = mcPart->production_vertex();
      if (!pvtx)
        continue;

      if (genVtxMap_->GetMit(pvtx->barcode()))
        continue;

      auto* vtx = vertexes_->Allocate();
      new (vtx) mithep::BaseVertex(pvtx->point3d().x() / 10.,
                                   pvtx->point3d().y() / 10.,
                                   pvtx->point3d().z() / 10.); // divide by 10 because HepMC uses mm

      genVtxMap_->Add(pvtx->barcode(), vtx);
    }
  }

  if (simActive_) {
    edm::Handle<edm::SimVertexContainer> hSimVertexProduct;
    GetProduct(simVerticesToken_, hSimVertexProduct, event);

    for (unsigned iV = 0; iV != hSimVertexProduct->size(); ++iV) {
      edm::SimVertexRef ref(hSimVertexProduct, iV);

      if (simVtxMap_->GetMit(ref))
        continue;

      auto& inVtx(*ref);

      auto* vtx = vertexes_->Allocate();
      new (vtx) mithep::BaseVertex(inVtx.position().x(),
                                   inVtx.position().y(),
                                   inVtx.position().z()); // divide by 10 because HepMC uses mm

      simVtxMap_->Add(ref, vtx);
    }
  }

  vertexes_->Trim();
}

DEFINE_MITHEP_TREEFILLER(FillerMCVertexes);
