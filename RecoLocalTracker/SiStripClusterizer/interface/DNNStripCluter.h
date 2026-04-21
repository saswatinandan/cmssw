#pragma once

class SiStripCluster;
class TrackerGeometry;
class DetId;
class StripTopology;

std::vector<std::vector<float>> DNNStripCluster(const SiStripCluster & cluster, const TrackerGeometry* tkGeom, const DetId& detId,
		const StripTopology& p, std::vector<std::string>& input_training_vars) {
  std::map<std::string, float> dnn_inputs;
  //dnn_inputs["dr_min_pixelTrk"] = dr_min_pixelTrk;
  uint16_t size = cluster.size();
  dnn_inputs["size"] = size;
  dnn_inputs["charge"] = cluster.charge();
 
  float       hitX[255];
  float       hitY[255];
  float       hitZ[255];
  uint16_t    firstStrip = cluster.firstStrip();
  uint16_t    endStrip = cluster.endStrip()+1;

  std::vector<int>adcs(4,0);
 
  for (int strip = firstStrip; strip < endStrip; ++strip) {
    if( (strip - firstStrip) < 4 )adcs.at(strip - firstStrip) = cluster[strip - firstStrip];
    GlobalPoint gp = (tkGeom->idToDet(detId))->surface().toGlobal(p.localPosition((float) strip));
    hitX   [strip - firstStrip] = gp.x();
    hitY   [strip - firstStrip] = gp.y();
    hitZ   [strip - firstStrip] = gp.z();
  }
  dnn_inputs["mean_x"] = std::accumulate(hitX, hitX+size, 0.0) / size;
  dnn_inputs["mean_y"] = std::accumulate(hitY, hitY+size, 0.0) / size;
  dnn_inputs["mean_z"] = std::accumulate(hitZ, hitZ+size, 0.0) / size;
  dnn_inputs["adc_at_idx_0"] = adcs[0];
  dnn_inputs["adc_at_idx_1"] = adcs[1];
  dnn_inputs["adc_at_idx_2"] = adcs[2];
  dnn_inputs["adc_at_idx_3"] = adcs[3]; 

  std::vector<float> inputvalues;
  for (auto & input_training_var : input_training_vars) {inputvalues.push_back(dnn_inputs.at(input_training_var));
//	  std::cout << "val " << input_training_var << dnn_inputs.at(input_training_var) << std::endl;
  }
  std::vector<std::vector<float>> ret;
  ret.emplace_back(inputvalues);
  return ret;
}
