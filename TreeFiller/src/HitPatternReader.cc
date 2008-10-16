// $Id: FillerTracks.cc,v 1.21 2008/10/13 10:41:36 bendavid Exp $

#include "MitProd/TreeFiller/interface/HitPatternReader.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
void HitPatternReader::InitLayerMap()
{
  // Initialize mapping between hit layer format in reco::HitPattern and the one used in
  // mithep::Track. Note in 21x stereo layers are treated separatelely.
  
  layerMap_[1160] = mithep::Track::PXB1;
  layerMap_[1168] = mithep::Track::PXB2;
  layerMap_[1176] = mithep::Track::PXB3;
  layerMap_[1288] = mithep::Track::PXF1;
  layerMap_[1296] = mithep::Track::PXF2;
  layerMap_[1416] = mithep::Track::TIB1;
  layerMap_[1420] = mithep::Track::TIB1S;
  layerMap_[1424] = mithep::Track::TIB2;
  layerMap_[1428] = mithep::Track::TIB2S;
  layerMap_[1432] = mithep::Track::TIB3;
  layerMap_[1440] = mithep::Track::TIB4;
  layerMap_[1544] = mithep::Track::TID1;
  layerMap_[1548] = mithep::Track::TID1S;
  layerMap_[1552] = mithep::Track::TID2;
  layerMap_[1556] = mithep::Track::TID2S;
  layerMap_[1560] = mithep::Track::TID3;
  layerMap_[1564] = mithep::Track::TID3S;
  layerMap_[1672] = mithep::Track::TOB1;
  layerMap_[1676] = mithep::Track::TOB1S;
  layerMap_[1680] = mithep::Track::TOB2;
  layerMap_[1684] = mithep::Track::TOB2S;
  layerMap_[1688] = mithep::Track::TOB3;
  layerMap_[1696] = mithep::Track::TOB4;
  layerMap_[1704] = mithep::Track::TOB5;
  layerMap_[1712] = mithep::Track::TOB6;
  layerMap_[1800] = mithep::Track::TEC1;
  layerMap_[1804] = mithep::Track::TEC1S;
  layerMap_[1808] = mithep::Track::TEC2;
  layerMap_[1812] = mithep::Track::TEC2S;
  layerMap_[1816] = mithep::Track::TEC3;
  layerMap_[1820] = mithep::Track::TEC3S;
  layerMap_[1824] = mithep::Track::TEC4;
  layerMap_[1828] = mithep::Track::TEC4S;
  layerMap_[1832] = mithep::Track::TEC5;
  layerMap_[1836] = mithep::Track::TEC5S;
  layerMap_[1840] = mithep::Track::TEC6;
  layerMap_[1844] = mithep::Track::TEC6S;
  layerMap_[1848] = mithep::Track::TEC7;
  layerMap_[1852] = mithep::Track::TEC7S;
  layerMap_[1856] = mithep::Track::TEC8;
  layerMap_[1860] = mithep::Track::TEC8S;
  layerMap_[1864] = mithep::Track::TEC9;
  layerMap_[1868] = mithep::Track::TEC9S;
}
