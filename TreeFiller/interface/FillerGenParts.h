//--------------------------------------------------------------------------------------------------
// $Id: FillerGenParts.h,v 1.5 2008/07/08 12:38:20 loizides Exp $
//
// FillerGenParts
//
// Imlementation of a filler to fill HepMC particles into mithep::GenParticles.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERGENPARTS_H
#define TREEFILLER_FILLERGENPARTS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerGenParts : public BaseFiller
  {
    public:
      FillerGenParts(const edm::ParameterSet &cfg, bool active=1);
      ~FillerGenParts();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
      const GenParticleMap *GetGenParticleMap() const { return genMap_; }
  
    private:
      std::string                         edmName_;
      std::string                         mitName_;
      mithep::GenParticleArr             *genParticles_; 
      mithep::GenParticleMap             *genMap_;
  };
}
#endif
