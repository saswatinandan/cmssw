/** SiPixelDecompressedClusterPrsoducer.cc
*/

// Our own stuff
#include "SiPixelDecompressedClusterProducer.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/DetId/interface/DetId.h"

// Framework
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "RecoLocalTracker/SiPixelClusterizer/plugins/PixelClusterizerBase.h"
// STL
#include <vector>
#include <memory>
#include <string>
#include <iostream>

// MessageLogger
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//---------------------------------------------------------------------------
//!  Constructor: set the ParameterSet and defer all thinking to setupClusterizer().
//---------------------------------------------------------------------------
SiPixelDecompressedClusterProducer::SiPixelDecompressedClusterProducer(edm::ParameterSet const& conf)
    : tPutPixelClusters(produces<SiPixelClusterCollectionNew>()) {

	tPixelClusters = consumes<SiPixelClusterCollectionNew>(conf.getParameter<edm::InputTag>("src"));    
}

// Destructor
SiPixelDecompressedClusterProducer::~SiPixelDecompressedClusterProducer() = default;

void SiPixelDecompressedClusterProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("src", edm::InputTag("siPixelDigis"));
  descriptions.add("SiPixelDecompressedClusterizerDefault", desc);
}

//---------------------------------------------------------------------------
//! The "Event" entrypoint: gets called by framework for every event
//---------------------------------------------------------------------------
void SiPixelDecompressedClusterProducer::produce(edm::Event& e, const edm::EventSetup& es) {
 
  //std::cout << "r:l:e: " << e.id().run() << ":" << e.id().event() << ":" << e.id().luminosityBlock() << std::endl;  	
  auto&  clusterCollection = e.get(tPixelClusters);
  auto output = std::make_unique<SiPixelClusterCollectionNew>();

  bool firstCluster(1);
  for (auto detClusters : clusterCollection) {

    int  prevMinPixelRow(0);
    edmNew::DetSetVector<SiPixelCluster>::FastFiller spc(*output, detClusters.detId());

    for(auto cluster : detClusters) {
     //if ( detClusters.id() != 303042565) continue;
     if ( !firstCluster )
	 cluster.settheMinPixelRow( cluster.minPixelRow() + prevMinPixelRow );
     firstCluster = 0;
     prevMinPixelRow = cluster.minPixelRow();
     PixelClusterizerBase::AccretionCluster cldata;
     int prev_r(0), prev_c(0);
     int old_minPixelRow = cluster.minPixelRow();
     int old_minPixelCol = cluster.minPixelCol();
     for(int size=0; size<cluster.size(); size++) {
       auto const newpix_adc = int(std::round(cluster.pixel(size).adc * 65535 / 32767.));
       auto r = (size==0) ? 0 : cluster.pixel(size).x + prev_r - cluster.minPixelRow();
       //std::cout << "prev_r: " << prev_r << "\t" << cluster.pixel(size).x << std::endl;
       uint16_t c = 0;
       if ( (size == 0) || !(r == prev_r) ) {
         c = cluster.pixel(size).y + cluster.minPixelCol();
       }
       else {
         c = cluster.pixel(size).y + prev_c - cluster.minPixelCol();
       }
       //std::cout << "r: " << r << "\t" << prev_r << std::endl;
       prev_r = r;
       prev_c = c;
       SiPixelCluster::PixelPos newpix(r, c);
       cldata.add(newpix, newpix_adc);
     }
     spc.push_back(SiPixelCluster(cldata.isize, cldata.adc, cldata.x, cldata.y, cldata.xmin, cldata.ymin, cluster.originalId(), true));
     spc.back().settheMinPixelRow(old_minPixelRow);
     spc.back().settheMinPixelCol(old_minPixelCol);
     std::cout << "minPixelRow: " << spc.back().minPixelRow() << "\t" << spc.back().minPixelCol() << std::endl;
    }
    std::cout << "detId: " << detClusters.id() << std::endl;
  }
  e.put(tPutPixelClusters, std::move(output));
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(SiPixelDecompressedClusterProducer);
