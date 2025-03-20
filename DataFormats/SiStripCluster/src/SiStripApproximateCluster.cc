#include "DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include <algorithm>
#include <cmath>
#include <assert.h>

SiStripApproximateCluster::SiStripApproximateCluster(const SiStripCluster& cluster,
                                                     unsigned int maxNSat,
                                                     float hitPredPos,
                                                     float& previous_cluster,
                                                     unsigned int& module_length,
                                                     unsigned int& previous_module_length,
                                                     bool peakFilter) {

  std::ostringstream current;
  current.precision(2);
  if (previous_cluster == -999.)
  {
    current << std::fixed << cluster.barycenter();
    std::string threshold = current.str();
    compBarycenter_ = std::stoi(threshold.substr(0,threshold.find(".")));
    compBarycenter_float = std::stoi(threshold.substr(threshold.find(".")+1));
  }
  else
  {
    current << std::fixed << (cluster.barycenter()-previous_cluster)+(module_length-previous_module_length);
    std::string threshold = current.str();
    compBarycenter_ = std::stoi(threshold.substr(0,threshold.find(".")));
    compBarycenter_float = std::stoi(threshold.substr(threshold.find(".")+1));
  }
   //compBarycenter_ = (barycenter_int-previous_cluster)+(module_length-previous_module_length);//std::round(((cluster.barycenter()-previous_cluster)+(module_length-previous_module_length))* maxRange_/maxBarycenter_);// + module_length;
  std::cout << ".cc compBarycenter_= " << compBarycenter_ << ", compBarycenter_float= " << int(compBarycenter_float) << ", barycenter= " << cluster.barycenter() <<  ", module length= " << module_length  << ", previous barycenter= " << previous_cluster << ", previous_module_length= " << previous_module_length << std::endl;
  //std::cout << ((cluster.barycenter()+module_length)-(previous_cluster+previous_module_length)) << std::endl;
  previous_cluster = barycenter(previous_cluster, module_length, previous_module_length);
  assert(cluster.barycenter() <= maxBarycenter_ && "Got a barycenter > maxBarycenter");
  //assert(compBarycenter_ <= maxRange_ && "Filling compBarycenter > maxRange");
  width_ = std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());
  double avgCharge_ = (cluster.charge() + width_/2)/ width_;
  assert(avgCharge_ <= maxavgCharge_ && "Got a avgCharge > maxavgCharge");
  compavgCharge_ = std::round(avgCharge_ * maxavgChargeRange_/maxavgCharge_);
  assert(compavgCharge_ <= maxavgChargeRange_ && "Filling compavgCharge > maxavgChargeRange");
  filter_ = false;
  isSaturated_ = false;
  peakFilter_ = peakFilter;

  //mimicing the algorithm used in StripSubClusterShapeTrajectoryFilter...
  //Looks for 3 adjacent saturated strips (ADC>=254)
  const auto& ampls = cluster.amplitudes();
  unsigned int thisSat = (ampls[0] >= 254), maxSat = thisSat;
  for (unsigned int i = 1, n = ampls.size(); i < n; ++i) {
    if (ampls[i] >= 254) {
      thisSat++;
    } else if (thisSat > 0) {
      maxSat = std::max<int>(maxSat, thisSat);
      thisSat = 0;
    }
  }
  if (thisSat > 0) {
    maxSat = std::max<int>(maxSat, thisSat);
  }
  if (maxSat >= maxNSat) {
    filter_ = true;
    isSaturated_ = true;
  }

  unsigned int hitStripsTrim = ampls.size();
  int sum = std::accumulate(ampls.begin(), ampls.end(), 0);
  uint8_t trimCut = std::min<uint8_t>(trimMaxADC_, std::floor(trimMaxFracTotal_ * sum));
  auto begin = ampls.begin();
  auto last = ampls.end() - 1;
  while (hitStripsTrim > 1 && (*begin < std::max<uint8_t>(trimCut, trimMaxFracNeigh_ * (*(begin + 1))))) {
    hitStripsTrim--;
    ++begin;
  }
  while (hitStripsTrim > 1 && (*last < std::max<uint8_t>(trimCut, trimMaxFracNeigh_ * (*(last - 1))))) {
    hitStripsTrim--;
    --last;
  }
  if (hitStripsTrim < std::floor(std::abs(hitPredPos) - maxTrimmedSizeDiffNeg_)) {
    filter_ = false;
  } else if (hitStripsTrim <= std::ceil(std::abs(hitPredPos) + maxTrimmedSizeDiffPos_)) {
    filter_ = true;
  } else {
    filter_ = peakFilter_;
  }
}
