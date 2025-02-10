#include "DataFormats/SiStripCluster/interface/SiStripApproximateClusterCollection.h"

void SiStripApproximateClusterCollection::reserve(std::size_t dets, std::size_t clusters) {
  detIds_.reserve(dets);
  beginIndices_.reserve(dets);
  clusters_.reserve(clusters);
}

SiStripApproximateClusterCollection::Filler SiStripApproximateClusterCollection::beginDet(unsigned int detId) {
  detIds_.push_back(detId);
  beginIndices_.push_back(0);
  return Filler(clusters_);
}
