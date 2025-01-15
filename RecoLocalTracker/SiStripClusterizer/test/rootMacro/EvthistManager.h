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

class EvthistManager
   : public histManagerBase
{
     public:
       EvthistManager(string obj):
       histManagerBase(obj) {

         ///// trk ///
	 hists["trk_pt"] = createhist(Form("%s_track_pt_tot", base_name.c_str()), "track_pt;track_pt;yield", numBins, customBins);
	 hists["trk_eta"] = createhist(Form("%s_track_eta", base_name.c_str()), "track_eta;track_eta;yield", 16, -2.4, 2.4);
	 hists["trk_dxyDdxyerr"] = createhist(Form("%s_track_dxyDdxyerr", base_name.c_str()), "track_dxyDdxyerr;xy/#sigma;yield", 30, -5., 5.);
         hists["trk_dzDdzerr"] = createhist(Form("%s_track_dzDdzerr", base_name.c_str()), "track_dzDdzerr;z/#sigma;yield", 50, 0., 10.);
         hists["trk_chi2"] = createhist(Form("%s_track_chi2", base_name.c_str()), "track_chi2;track_chi2;yield", 100, 0,20);
         hists["trk_nhits"] = createhist(Form("%s_track_nhits", base_name.c_str()), "track_nhits;track_nhits;yield", 100, -0.5, 99.5);
         hists["trk_pterrDpt"] = createhist(Form("%s_track_pterrDpt", base_name.c_str()), "track_pterrDpt;track_pTErr/pt;yield", 50, 0, 1);
         for (int i=TrackAlgorithm::undefAlgorithm; i<=TrackAlgorithm::displacedRegionalStep; i++) {
           std::string name = "trk_cutflow_"+to_string(i);
           hists[name] = createhist(Form("%s_%s", base_name.c_str(), name.c_str()), Form("%s;cutflow;yield", algoNames[i].c_str()), trk_cuts::nhits, trk_cuts::nocut, trk_cuts::nhits+1);
           hists[name]->GetXaxis()->SetLabelSize(0.025);
           for(int ibin=trk_cuts::nocut; ibin<=trk_cuts::nhits; ibin++)
	     hists[name]->GetXaxis()->SetBinLabel(ibin, trk_cutToname[ibin].c_str());
        }
	hists_2d["trk_eta_phi"] = createhist(Form("%s_track_eta_phi", base_name.c_str()), "track_eta_phi;#eta; #phi;yield", 16, -2.4, 2.4, 30, -TMath::Pi(), TMath::Pi());

       ///// jet ////

       hists["jet_pt"]      = createhist(Form("%s_jet_pt_tot", base_name.c_str()), "jet_pt;jet_pt;yield", 20, 20, 100);
       hists["jet_eta"]     = createhist(Form("%s_jet_eta", base_name.c_str()), "jet_eta;jet_eta;yield", 16, -2.4, 2.4);
       hists["jet_cutflow"] = createhist(Form("%s_jet_cutflow", base_name.c_str()), "cutflow;cutflow;yield", jet_cuts::eta, jet_cuts::nocut, jet_cuts::eta+1);
       for(int ibin=jet_cuts::nocut; ibin<=jet_cuts::eta; ibin++)
           hists["jet_cutflow"]->GetXaxis()->SetBinLabel(ibin, jet_cutToname[ibin].c_str());
       };
};


#endif //EVTHIST_MANAGER
