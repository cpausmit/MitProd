//--------------------------------------------------------------------------------------------------
// $Id: FillerDecayParts.h,v 1.4 2008/09/10 03:30:22 loizides Exp $
//
// FillerDecayParts
//
// Imlementation of a filler to copy the mitedm::DecayPart collection into the
// mithep::DecayParticle collection.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERDECAYPARTS_H
#define MITPROD_TREEFILLER_FILLERDECAYPARTS_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/Collections.h"

namespace mithep 
{
  class FillerDecayParts : public BaseFiller
  {  
    public:
      FillerDecayParts(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerDecayParts();
    
      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
    
    private:
      std::string                    edmName_;         //edm name of decay parts collection
      std::string                    mitName_;         //name of DecayParticles in OAK
      std::vector<std::string>       basePartMapNames_; //name of imported maps wrt stable parts
      std::vector<const mithep::BasePartMap*> basePartMaps_;     //maps wrt stable parts
      mithep::DecayParticleArr      *decays_;          //array of DecayParticles
  };
}
#endif
