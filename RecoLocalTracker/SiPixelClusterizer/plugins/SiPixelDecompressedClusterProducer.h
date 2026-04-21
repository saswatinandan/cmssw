#ifndef RecoLocalTracker_SiPixelClusterizer_SiPixelDecompressedClusterProducer_h
#define RecoLocalTracker_SiPixelClusterizer_SiPixelDecompressedClusterProducer_h

//---------------------------------------------------------------------------
//! \class SiPixelDecompressedClusterProducer
//---------------------------------------------------------------------------

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/InputTag.h"

class dso_hidden SiPixelDecompressedClusterProducer final : public edm::stream::EDProducer<> {
public:
  //--- Constructor, virtual destructor (just in case)
  explicit SiPixelDecompressedClusterProducer(const edm::ParameterSet& conf);
  ~SiPixelDecompressedClusterProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  //--- The top-level event method.
  void produce(edm::Event& e, const edm::EventSetup& c) override;

private:
  edm::EDGetTokenT<SiPixelClusterCollectionNew> tPixelClusters;
  edm::EDPutTokenT<SiPixelClusterCollectionNew> tPutPixelClusters;
};

#endif
