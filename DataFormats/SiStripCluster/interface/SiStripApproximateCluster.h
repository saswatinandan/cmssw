#ifndef DataFormats_SiStripCluster_SiStripApproximateCluster_h
#define DataFormats_SiStripCluster_SiStripApproximateCluster_h

#include "FWCore/Utilities/interface/typedefs.h"
#include "assert.h"
#include <iostream>

class SiStripCluster;
class SiStripApproximateCluster {
public:
  SiStripApproximateCluster() {}

  explicit SiStripApproximateCluster(cms_uint16_t compBarycenter,
                                     cms_uint8_t width,
                                     cms_uint8_t compavgCharge,
                                     bool filter,
                                     bool isSaturated,
                                     bool peakFilter = false)
      : compBarycenter_(compBarycenter),
        width_(width),
        compavgCharge_(compavgCharge),
        filter_(filter),
        isSaturated_(isSaturated),
        peakFilter_(peakFilter) {}

  explicit SiStripApproximateCluster(const SiStripCluster& cluster,
                                     unsigned int maxNSat,
                                     float hitPredPos,
                                     float& previous_cluster,
                                     bool peakFilter);

  float barycenter() const { 
    float _barycenter = compBarycenter_ * maxBarycenter_/maxRange_ ;
    assert(_barycenter <= maxBarycenter_ && "Returning barycenter > maxBarycenter");
    return _barycenter; }
  cms_uint8_t width() const {return width_; }
  cms_uint8_t avgCharge() const { 
     float avgCharge_ = compavgCharge_ * maxavgCharge_/maxavgChargeRange_ ;
    assert(avgCharge_ <= maxavgCharge_ && "Returning avgCharge > maxavgCharge");
     return avgCharge_; }
  bool filter() const { return filter_; }
  bool isSaturated() const { return isSaturated_; }
  bool peakFilter() const { return peakFilter_; }

private:
  cms_uint16_t compBarycenter_ = 0;
  cms_uint8_t width_ = 0;
  cms_uint8_t compavgCharge_ = 0;
  bool filter_ = false;
  bool isSaturated_ = false;
  bool peakFilter_ = false;
  static constexpr double maxRange_ = 65535; //65535; //65535;
  static constexpr double maxBarycenter_ = 768.;
  static constexpr double maxavgChargeRange_ = 255; //255; //255;
  static constexpr double maxavgCharge_ = 255.;
  static constexpr double trimMaxADC_ = 30.;
  static constexpr double trimMaxFracTotal_ = .15;
  static constexpr double trimMaxFracNeigh_ = .25;
  static constexpr double maxTrimmedSizeDiffNeg_ = .7;
  static constexpr double maxTrimmedSizeDiffPos_ = 1.;
};
#endif  // DataFormats_SiStripCluster_SiStripApproximateCluster_h
