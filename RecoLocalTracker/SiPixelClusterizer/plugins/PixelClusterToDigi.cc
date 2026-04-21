#include <memory>

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/SiPixelRawData/interface/SiPixelRawDataError.h"
#include "DataFormats/SiPixelDigi/interface/SiPixelDigiConstants.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h" 
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"

class PixelClusterToDigi : public edm::stream::EDProducer<> {
public:
  /// ctor
  explicit PixelClusterToDigi(const edm::ParameterSet&);

   edm::InputTag inputTagClusters;
   // Event Data
   edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster>> clusterToken;
  /// dtor
  ~PixelClusterToDigi() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  /// get data, convert to digis attach againe to Event
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  edm::ParameterSet config_;
  const edm::EDPutTokenT<edm::DetSetVector<PixelDigi>> siPixelDigiCollectionToken_;
};

// -----------------------------------------------------------------------------
PixelClusterToDigi::PixelClusterToDigi(const edm::ParameterSet& conf) :
 siPixelDigiCollectionToken_{produces<edm::DetSetVector<PixelDigi>>("new")}
 {
   inputTagClusters       = conf.getParameter<edm::InputTag>("InputLabel");
   clusterToken           = consumes<edmNew::DetSetVector<SiPixelCluster>>(inputTagClusters);
 }

// -----------------------------------------------------------------------------
PixelClusterToDigi::~PixelClusterToDigi() {
  edm::LogInfo("PixelClusterToDigi") << " HERE ** PixelClusterToDigi destructor!";
}

void PixelClusterToDigi::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("InputLabel", edm::InputTag("siPixelRawData"));
  descriptions.add("siPixelClusterToDigi", desc);
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
void PixelClusterToDigi::produce(edm::Event& ev, const edm::EventSetup& es) {
  auto digis = edm::DetSetVector<PixelDigi>();

  edm::Handle<edmNew::DetSetVector<SiPixelCluster>> clusterCollection = ev.getHandle(clusterToken);
  edm::DetSet<PixelDigi>* detDigis = nullptr;
  for (const auto& detClusters : *clusterCollection) {
      uint32_t rawId  = detClusters.id();
      detDigis = &digis.find_or_insert(rawId);
      if ((*detDigis).empty()) {
	 (*detDigis).data.reserve(32);  // avoid the first relocations
      }
      for(const auto& cluster : detClusters) {
         //std::cout << cluster.size() << "\t" << cluster.charge() << std::endl;
         for(int s=0; s<cluster.size(); s++) { 
            if (detDigis) {
		(*detDigis).data.emplace_back(cluster.pixel(s).x, cluster.pixel(s).y, cluster.pixel(s).adc);
            }
	 }
      }
  }
  ev.emplace(siPixelDigiCollectionToken_, std::move(digis));
}
// declare this as a framework plugin
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PixelClusterToDigi);
