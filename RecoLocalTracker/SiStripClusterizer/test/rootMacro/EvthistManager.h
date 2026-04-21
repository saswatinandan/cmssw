#ifndef EVTHIST_MANAGER_H
#define EVTHIST_MANAGER_H

#include "histManagerBase.h"
#include "TrackAlgo.h"
#include "TMath.h"

namespace trk_cuts {
enum  {nocut=1, chi2, ptRes, nhits};
}
map<int, std::string> trk_cutToname = { {trk_cuts::nocut, "nocut"}, {trk_cuts::chi2, "normalizedchi2<2"}, {trk_cuts::ptRes, "abs(pTErr/pT)<0.10"}, {trk_cuts::nhits, "nhits>=11"}};

namespace jet_cuts{
enum jet_cuts {nocut=1, pt, eta};
}
map<int, std::string> jet_cutToname = { {jet_cuts::nocut, "nocut"}, {jet_cuts::pt, "pt>20"}, {jet_cuts::eta, "|eta|<2.4"}};

constexpr Double_t array_displaced_xy[] = {0,0.5, 1, 1.5,2,3,4,5, 10,20};
constexpr Double_t array_displaced_z[] = {0,1.0,1.5,2,3,4,5,6,7,8,9,10, 15, 20, 30, 40, 50, 70};
constexpr Double_t array_xy[] = {0,0.,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,1,1.2,1.4,2};
constexpr Double_t array_z[] = {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,1,1.2, 1.4,1.6,1.8,2,2.5,3,3.5,4.5,5,6,10};
constexpr Int_t displaced_trk_algo[] = {8, 9, 10, 26};

class EvthistManager
   : public histManagerBase
{
     public:
       EvthistManager(string obj):
       histManagerBase(obj) {

         ///// trk ///
	 hists["trk_pt"] = createhist(Form("%s_track_pt_tot", base_name.c_str()), "track_pt;track_pt;yield", numBins, customBins);
	 hists["trk_eta"] = createhist(Form("%s_track_eta", base_name.c_str()), "track_eta;track_eta;yield", 16, -2.4, 2.4);
       };
};


#endif //EVTHIST_MANAGER
