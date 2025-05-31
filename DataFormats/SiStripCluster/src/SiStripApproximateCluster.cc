#include "DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <bitset>
uint8_t reverseByte(uint8_t b) {
   uint8_t rev = 0;
   for (int i = 0; i < 8; ++i) {
	rev <<= 1;
	rev |= (b & 1);
        b >>= 1;
    }
   return rev;
}
void printBits_16(uint16_t value) {
   //Method 1: Using std::bitset (easy and clean)
   std::bitset<16> bits(value);
   std::cout << bits << std::endl;
}
void printBits_8(uint8_t value) {
    // Method 1: Using std::bitset (easy and clean)
    std::bitset<8> bits(value);
    std::cout << bits << std::endl;}
void printBits_32(uint32_t value) {
    // Method 1: Using std::bitset (easy and clean)
    std::bitset<32> bits(value);
    std::cout << bits << std::endl;}
int bits_used(uint8_t x) {
	for (int i = 7; i >= 0; --i) {
          //std::cout << i << "\t" << (int)x << "\t" << (x & (1 << i)) << std::endl;
	  if (x & (1 << i)) return i + 1;
	}
	return 0;
}

uint16_t compress(uint8_t a, uint8_t b) {
	int bits_a = bits_used(a);
	int bits_b = bits_used(b);
	// Shift b to make room for a in the lower bits
	uint16_t packed = ((uint16_t)b << bits_a) | a;
	return packed;
}
SiStripApproximateCluster::SiStripApproximateCluster(const SiStripCluster& cluster,
                                                     unsigned int maxNSat,
                                                     float hitPredPos,
                                                     float& previous_cluster,
                                                     unsigned int& module_length,
                                                     unsigned int& previous_module_length,
                                                     bool peakFilter) {
  //cms_uint16_t compBarycenter_=0;
  cms_uint8_t width_ = 0;
  cms_uint8_t compavgCharge_ = 0;
  if (previous_cluster == -999.)
   compBarycenter_ = std::round(cluster.barycenter() * maxRange_/maxBarycenter_);
  else
   compBarycenter_ = std::round(((cluster.barycenter()-previous_cluster)+(module_length-previous_module_length))* maxRange_/maxBarycenter_);// + module_length;
  //std::cout << ".cc compBarycenter_= " << compBarycenter_ << ", barycenter= " << cluster.barycenter() <<  ", module length= " << module_length  << ", previous barycenter= " << previous_cluster << ", previous_module_length= " << previous_module_length << std::endl;
  //std::cout << ((cluster.barycenter()+module_length)-(previous_cluster+previous_module_length)) << std::endl;
  previous_cluster = barycenter(previous_cluster, module_length, previous_module_length);
  assert(cluster.barycenter() <= maxBarycenter_ && "Got a barycenter > maxBarycenter");
  assert(compBarycenter_ <= maxRange_ && "Filling compBarycenter > maxRange");
  width_ = std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());//std::min(255,(int)cluster.size());
  cms_uint8_t avgCharge_ = cluster.charge() / width_;
  assert(avgCharge_ <= maxavgCharge_ && "Got a avgCharge > maxavgCharge");
  compavgCharge_ = std::round(avgCharge_ * maxavgChargeRange_/maxavgCharge_);
  assert(compavgCharge_ <= maxavgChargeRange_ && "Filling compavgCharge > maxavgChargeRange");
  filter_ = false;
  isSaturated_ = false;
  peakFilter_ = peakFilter;
  uint8_t lsb = compBarycenter_ & 0xFF;
  uint8_t hsb = (compBarycenter_ >> 8) & 0xFF;
  all = compress(width_,compavgCharge_);
  //all |= ((uint32_t)lsb) <<24;
  uint8_t rev_ = reverseByte(compavgCharge_);
  //all |= ((uint16_t)rev_) << 8;
  //all |= width_;
  //uint8_t rev_ = reverseByte(width_);
  //all |= ((uint32_t)rev_)<<16;
  //all |= ((uint32_t)lsb)<<16;
  //all |= ((uint32_t)rev_)<<8;
  //all |= ((uint32_t)hsb)<<8;
  uint8_t revc_ = reverseByte(compavgCharge_);
  //all |=compavgCharge_;
  //all |= hsb;
  //std::cout << "bary: " << compBarycenter_ << std::endl;
  //printBits_16(compBarycenter_);
  //std::cout << "width: " << (int)width_ << std::endl;
  //printBits_8(width_);
  //std::cout << "charge: " << (int)compavgCharge_ << std::endl;
  //printBits_8(compavgCharge_);
  //std::cout << "all " << std::endl;
  //printBits_16(all);
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
