//--------------------------------------------------------------------------------------------------
// $Id: FillerGenParts.h,v 1.1 2008/06/24 14:58:08 loizides Exp $
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
#include "MitAna/DataTree/interface/GenParticle.h"
#include "MitAna/DataTree/interface/Array.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerGenParts : public BaseFiller
  {
    public:
      FillerGenParts(const edm::ParameterSet &cfg);
      ~FillerGenParts();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                         edmName_;
      std::string                         mitName_;
      mithep::Array<mithep::GenParticle> *genParticles_;  
  };
}
#endif
