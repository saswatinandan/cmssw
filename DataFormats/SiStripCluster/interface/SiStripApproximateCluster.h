#ifndef DataFormats_SiStripCluster_SiStripApproximateCluster_h
#define DataFormats_SiStripCluster_SiStripApproximateCluster_h

#include "FWCore/Utilities/interface/typedefs.h"
#include "assert.h"
#include <iostream>
#include <sstream>

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
                                     unsigned int& module_length,
                                     unsigned int& previous_module_length,
                                     bool peakFilter);

  float barycenter(float previous_barycenter=0,
                   unsigned int module_length=0, unsigned int previous_module_length=0) const {
    float _barycenter;
    float compress_barycenter = std::stof(std::to_string(compBarycenter_)+"."+std::to_string(int(compBarycenter_float)));
    if (  previous_barycenter == -999 )
      _barycenter = compress_barycenter;//ompBarycenter_ * maxBarycenter_/maxRange_;
    else {
      _barycenter = (compress_barycenter+previous_barycenter)-(module_length-previous_module_length);//)((compBarycenter_ * maxBarycenter_/maxRange_) - (module_length-previous_module_length)) + previous_barycenter;
    }
    //std::cout << ".h compBarycenter_= " << compBarycenter_ << ", barycenter= " << _barycenter << ", module length= " << module_length  << ", previous barycenter= " << previous_barycenter << ", previous_module_length= " << previous_module_length << std::endl;
  //std::cout << _barycenter << "\t" << compress_barycenter << "\t" << previous_barycenter << "\t" << module_length << "\t" << previous_module_length << std::endl;
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
  cms_uint8_t compBarycenter_float = 0;
  cms_uint8_t width_ = 0;
  cms_uint8_t compavgCharge_ = 0;
  bool filter_ = false;
  bool isSaturated_ = false;
  bool peakFilter_ = false;
  static constexpr double maxRange_ = 32767; //32767; //32767; //32767; //16383; //16383; //32767;
  static constexpr double maxBarycenter_ = 1536.;
  static constexpr double maxavgChargeRange_ = 63; //63; //63; //255; //255; //255; //255;
  static constexpr double maxavgCharge_ = 255.;
  static constexpr double trimMaxADC_ = 30.;
  static constexpr double trimMaxFracTotal_ = .15;
  static constexpr double trimMaxFracNeigh_ = .25;
  static constexpr double maxTrimmedSizeDiffNeg_ = .7;
  static constexpr double maxTrimmedSizeDiffPos_ = 1.;
};
#endif  // DataFormats_SiStripCluster_SiStripApproximateCluster_h
