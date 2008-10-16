//--------------------------------------------------------------------------------------------------
// $Id: FillerTracks.h,v 1.16 2008/10/13 10:41:36 bendavid Exp $
//
// HitPatternReader
//
// Helper class to translate track hits from the format they are stored in within reco::HitPattern
// to the corresponding mithep::Track::EHitLayer
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_HITPATTERNREADER_H
#define MITPROD_TREEFILLER_HITPATTERNREADER_H

#include <map>
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MitAna/DataTree/interface/Track.h"


namespace mithep 
{
  class HitPatternReader
  {  
    public:
      HitPatternReader() : layerMask_(0xFFFFFFFC) { InitLayerMap(); }
      virtual ~HitPatternReader() { layerMap_.clear(); }
      
      mithep::Track::EHitLayer Layer(const uint32_t &hit) const;

    protected:
      void                                         InitLayerMap();
    
      const uint32_t                               layerMask_; //needed for bitwise operations
      std::map<uint32_t,mithep::Track::EHitLayer>  layerMap_;        //hitlayer bitmap map

  };
}

//--------------------------------------------------------------------------------------------------
inline mithep::Track::EHitLayer mithep::HitPatternReader::Layer(const uint32_t &hit) const
{
  std::map<uint32_t,mithep::Track::EHitLayer>::const_iterator iter
        = layerMap_.find(hit & layerMask_);

  if (iter != layerMap_.end())
    return iter->second;
  else throw edm::Exception(edm::errors::Configuration, "HitPatternReader::Layer()\n")
         << "Error! Layer not found corresponding to hit pattern (" << hit << "." << std::endl; 
}
#endif
